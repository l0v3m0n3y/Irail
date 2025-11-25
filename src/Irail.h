#ifndef IRAIL_H
#define IRAIL_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Irail {
private:
    std::string api_base = "https://api.irail.be/v1";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("api.irail.be"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                // Кодируем значения параметров
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }
public:
    Irail(){
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> get_stations(const std::string& lang="") {
        std::map<std::string, std::string> params;
        params["format"] = "json";
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        return make_api_call("/stations" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_liveboard(const std::string& lang="",const std::string& date="",const std::string& time="",bool alerts=false,const std::string& arrdep="",const std::string& id="",const std::string& station="") {
        std::map<std::string, std::string> params;
        params["format"] = "json";
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!date.empty()) params["date"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(date)));
        if (!time.empty()) params["time"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(time)));
        if (!alerts) params["alerts"] = alerts;
        if (!arrdep.empty()) params["arrdep"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(arrdep)));
        if (!id.empty()) params["id"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(id)));
        if (!station.empty()) params["station"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(station)));
        return make_api_call("/liveboard" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_connections(const std::string& lang="",const std::string& date="",const std::string& time="",bool alerts=false,int results=0,const std::string& typeOfTransport="",const std::string& timesel="",const std::string& to="",const std::string& from="") {
        std::map<std::string, std::string> params;
        params["format"] = "json";
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!date.empty()) params["date"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(date)));
        if (!time.empty()) params["time"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(time)));
        if (!alerts) params["alerts"] = alerts;
        if (results>0) params["results"] =std::to_string(results)
        if (!typeOfTransport.empty()) params["typeOfTransport"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(typeOfTransport)));
        if (!timesel.empty()) params["timesel"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(timesel)));
        if (!to.empty()) params["to"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(to)));
        if (!from.empty()) params["from"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(from)));
        return make_api_call("/connections" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_vehicle(const std::string& lang="",const std::string& date="",bool alerts=false,const std::string& id="") {
        std::map<std::string, std::string> params;
        params["format"] = "json";
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!date.empty()) params["date"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(date)));
        if (!alerts) params["alerts"] = alerts;
        if (!id.empty()) params["id"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(id)));
        return make_api_call("/vehicle" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_composition(const std::string& lang="",const std::string& date="",const std::string& id="") {
        std::map<std::string, std::string> params;
        params["format"] = "json";
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!date.empty()) params["date"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(date)));
        if (!id.empty()) params["id"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(id)));
        return make_api_call("/composition" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_disturbances(const std::string& lang="",const std::string& lineBreakCharacter="") {
        std::map<std::string, std::string> params;
        params["format"] = "json";
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!lineBreakCharacter.empty()) params["lineBreakCharacter"] = utility::conversions::to_utf8string( web::uri::encode_data_string(utility::conversions::to_string_t(lineBreakCharacter)));
        return make_api_call("/disturbances" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_logs() {
        std::map<std::string, std::string> params;
        params["format"] = "json";
        return make_api_call("/logs" + build_query_params(params),"GET");
    }
};

#endif
