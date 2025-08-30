

#include <userver/server/handlers/ping.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utest/using_namespace_userver.hpp>
 
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/statistics_storage.hpp>
#include <userver/components/tcp_acceptor_base.hpp>
#include <userver/concurrent/queue.hpp>
#include <userver/server/handlers/server_monitor.hpp>
#include <userver/utils/daemon_run.hpp>
 
#include <userver/server/websocket/websocket_handler.hpp>

#include <userver/utils/statistics/metric_tag.hpp>
#include <userver/utils/statistics/metrics_storage.hpp>
 
#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>

#include <userver/formats/json.hpp>

#include <userver/crypto/base64.hpp>
 
#include <string>
#include <iostream>

namespace samples::websocket {
 
class WebsocketsHandler final : public server::websocket::WebsocketHandlerBase {
public:
    // `kName` is used as the component name in static config
    static constexpr std::string_view kName = "websocket-entry";
 
    // Component is valid after construction and is able to accept requests
    using WebsocketHandlerBase::WebsocketHandlerBase;
     
    WebsocketsHandler(const components::ComponentConfig& config, const components::ComponentContext& context)
     : server::websocket::WebsocketHandlerBase(config, context),
     pg_cluster_(context.FindComponent<components::Postgres>("postgres-db-1").GetCluster()) {}

    void Handle(server::websocket::WebSocketConnection& chat, server::request::RequestContext&) const override {
        server::websocket::Message message;
        formats::json::Value json;
        //formats::json::Value json = formats::json::FromString(R"({"key": "value"})");
        //server::websocket::Message
        
        while (!engine::current_task::ShouldCancel()) {
            chat.Recv(message);               // throws on closed/dropped connection            
            if (message.close_status) break;  // explicit close if any
            
            for (int i{}; i < (int)message.data.size() - 1; i++) {
                if (message.data[i] == ' ') {                     
                    //json = formats::json::FromString(R"({"login":, "password": ""})");
                    json = formats::json::MakeObject(
                        "login", message.data.substr(0, i),
                        "password", crypto::base64::Base64Encode(message.data.substr(i + 1, (int)message.data.size() - (i + 1) - 4)),                        
                        "entry_type", message.data.substr((int)message.data.size() - 3, 3)
                    );                    
                    //std::cout << message.data.substr(i + 1, (int)message.data.size() - (i + 1) - 4) << "\n";
                    //std::cout << crypto::base64::Base64Encode(message.data.substr(i + 1, (int)message.data.size() - (i + 1) - 4)) << "\n";
                    break;
                }
            }            
            if (json["entry_type"].As<std::string>() == "LOG") {         
                if (!pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave, 
                    "SELECT password FROM EDOGS.users WHERE login=$1", json["login"].As<std::string>()).IsEmpty()) {
                    const auto res = pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave, 
                    "SELECT password FROM EDOGS.users WHERE login=$1", json["login"].As<std::string>());
                
                    if (res.AsSingleRow<std::string>() == json["password"].As<std::string>()) {                    
                    message.data = "ACCESS";

                    if (pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave, 
                    "SELECT COUNT(*) FROM EDOGS.docs WHERE login=$1", json["login"].As<std::string>()).AsSingleRow<int>() > 0) {
                        const storages::postgres::ResultSet res = 
                            pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave,     
                        "SELECT title FROM EDOGS.docs WHERE login=$1",
                        json["login"].As<std::string>());

                        std::string output{};
                        std::vector<std::string> vec = res.AsContainer<std::vector<std::string>>();
                        message.data = "";
                        for (auto& elem : vec) {
                            message.data += "#" + elem;
                            std::cout << "#" << elem;
                        }                        
                        std::cout << "\n";               
                        /*for (auto row : res.AsSetOf<std::tuple<std::string, std::string>>(storages::postgres::kRowTag)) {
                            static_assert(std::is_same_v<decltype(row), std::tuple<std::string, std::string>>,
                        "Iterate over tuples");
                        auto [a, b] = row;
                            output += a;
                            output += " ";
                            output += b;
                            output += "\n";
                        }*/
                    }

                    chat.Send(std::move(message));

                    }else {
                        message.data = "DENIED";
                        chat.Send(std::move(message));
                    }     
                }                                
                else {
                    message.data = "DENIED";
                    chat.Send(std::move(message));
                }

            }else if (json["entry_type"].As<std::string>() == "REG") {
                storages::postgres::Transaction transaction = pg_cluster_->Begin("sample_transaction_insert_key_value", storages::postgres::ClusterHostType::kMaster, {});
                auto res = transaction.Execute(
                    "INSERT INTO EDOGS.users (login, password)"
                    "VALUES ($1, $2)"
                    "ON CONFLICT DO NOTHING", json["login"].As<std::string>(), json["password"].As<std::string>());
                if (res.RowsAffected()) {
                    transaction.Commit();
                    message.data = "ACCESS";
                    chat.Send(std::move(message));                              
                }
                transaction.Rollback();
            }
            //std::cout << json["login"].As<std::string>() << " " << json["password"].As<std::string>() << " " << json["entry_type"].As<std::string>() << "\n";
            chat.Send(std::move(message));    // throws on closed/dropped connection
        }
        if (message.close_status) chat.Close(*message.close_status);
    }

private:
     storages::postgres::ClusterPtr pg_cluster_;
};

class WebsocketsEditHandler final : public server::websocket::WebsocketHandlerBase {
public:
    // `kName` is used as the component name in static config
    static constexpr std::string_view kName = "websocket-edit";
 
    // Component is valid after construction and is able to accept requests
    using WebsocketHandlerBase::WebsocketHandlerBase;
     
    WebsocketsEditHandler(const components::ComponentConfig& config, const components::ComponentContext& context)
     : server::websocket::WebsocketHandlerBase(config, context),
     pg_cluster_(context.FindComponent<components::Postgres>("postgres-db-1").GetCluster()) {}

    void Handle(server::websocket::WebSocketConnection& chat, server::request::RequestContext&) const override {
        server::websocket::Message message;
        formats::json::Value json;                
        
        while (!engine::current_task::ShouldCancel()) {
            chat.Recv(message);               // throws on closed/dropped connection            
            if (message.close_status) break;  // explicit close if any
            if (message.data[0] == '*') {
                std::cout << "Existing document event\n";
                size_t first_pos = message.data.find('#');
                size_t second_pos = message.data.find('#', message.data.find('#') + 1);
                json = formats::json::MakeObject(                        
                    "login", message.data.substr(1, first_pos - 1),
                    "DocsName", message.data.substr(first_pos + 1, second_pos - first_pos - 1),
                    "text", message.data.substr(second_pos + 1, message.data.size() - second_pos - 1)                    
                );    
                std::cout << json["login"].As<std::string>() << " " << json["DocsName"].As<std::string>() << " " << json["text"].As<std::string>() << "\n";
                if (!pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave, 
                        "SELECT id FROM EDOGS.docs WHERE login=$1 AND title=$2", json["login"].As<std::string>(), json["DocsName"].As<std::string>()).IsEmpty()) {
                    message.data = '*';
                    message.data += pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave,
                    "SELECT data FROM EDOGS.docs WHERE login=$1 AND title=$2", json["login"].As<std::string>(), json["DocsName"].As<std::string>()).AsSingleRow<std::string>();
                    std::cout << "success\n";
                    chat.Send(std::move(message));
                }
            }
            else {
                std::cout << "New document event\n";
                size_t first_pos = message.data.find('#');
                size_t second_pos = message.data.find('#', message.data.find('#') + 1);
                json = formats::json::MakeObject(                        
                    "login", message.data.substr(0, first_pos),
                    "DocsName", message.data.substr(first_pos + 1, second_pos - first_pos - 1),
                    "text", message.data.substr(second_pos + 1, message.data.size() - second_pos - 1)                    
                );                                        
            
            
            
                if (pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave, 
                        "SELECT id FROM EDOGS.docs WHERE login=$1 AND title=$2", json["login"].As<std::string>(), json["DocsName"].As<std::string>()).IsEmpty()) { //Have to create a new document in the database
                
                    storages::postgres::Transaction transaction = pg_cluster_->Begin("sample_transaction_insert_key_value", storages::postgres::ClusterHostType::kMaster, {});
                    int countOfTable = transaction.Execute("SELECT COUNT(*) FROM EDOGS.docs").AsSingleRow<int>();
                    std::cout << "REG\n";
                    std::cout << countOfTable << " " << json["login"].As<std::string>() << " " << json["DocsName"].As<std::string>() << " " << json["text"].As<std::string>() << "\n";
                    auto res = transaction.Execute(
                        "INSERT INTO EDOGS.docs(id, login, title, data)"
                        "VALUES($1, $2, $3, $4)"
                        "ON CONFLICT DO NOTHING", countOfTable + 1, json["login"].As<std::string>(), json["DocsName"].As<std::string>(), "");
                    if (res.RowsAffected()) {
                        transaction.Commit();
                        message.data = "Created a new Document";
                        chat.Send(std::move(message));                              
                    }
                /*message.data = pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave, "SELECT data FROM EDOGS.docs WHERE login=$1 and title=$2",
                json["login"].As<std::string>(), json["DocsName"].As<std::string>()).AsSingleRow<std::string>();
                chat.Send(std::move(message));*/
                    transaction.Rollback();

                }else {//Updating the present data
                    std::cout << "UPDATE\n";
                    storages::postgres::Transaction transaction =
                    pg_cluster_->Begin("sample_transaction_change_key_value", storages::postgres::ClusterHostType::kMaster, {});
 
                    auto res = transaction.Execute(
                        "UPDATE EDOGS.docs SET data=$1 WHERE login=$2 AND title=$3;",
                    json["text"].As<std::string>(), json["login"].As<std::string>(), json["DocsName"].As<std::string>());

                    if (res.RowsAffected()) {
                        transaction.Commit();
                        message.data = "The document is saved";
                        chat.Send(std::move(message)); 
                    }
                /*message.data = pg_cluster_->Execute(storages::postgres::ClusterHostType::kSlave, "SELECT data FROM EDOGS.docs WHERE login=$1 and title=$2",
                json["login"].As<std::string>(), json["DocsName"].As<std::string>()).AsSingleRow<std::string>();
                chat.Send(std::move(message));*/
                    transaction.Rollback();
                }

                chat.Send(std::move(message));    // throws on closed/dropped connection
            }
            
        }
        if (message.close_status) chat.Close(*message.close_status);
    }

private:
     storages::postgres::ClusterPtr pg_cluster_;
};
 
}  // namespace samples::websocket


int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
  .Append<userver::server::handlers::Ping>()
  .Append<userver::components::TestsuiteSupport>()
  .Append<userver::components::HttpClient>()
  .Append<userver::server::handlers::TestsControl>()
  .Append<userver::clients::dns::Component>()
  .Append<userver::components::Postgres>("postgres-db-1")
  .Append<samples::websocket::WebsocketsHandler>()
  .Append<samples::websocket::WebsocketsEditHandler>();

  //.Append<server::handlers::ServerMonitor>()
  //.Append<samples::tcp::echo::Echo>();
  
  
  //.Append<samples::tcp::Hello>();
                                                                                                                                                                                                                                                                      
  return userver::utils::DaemonMain(argc, argv, component_list);
}
