#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/http_headers.h>
#include <cpprest/uri_builder.h>
#include <cpprest/json.h>

pplx::task<web::json::value> task_from_response(web::http::http_response response)
{
    if (response.status_code() == web::http::status_codes::OK)
    {
        return response.extract_json();
    }
    std::cerr << "Response status code: " << response.status_code() << "\n";
    std::abort();
}

void print_cat_breeds_from_task(pplx::task<web::json::value> task)
{   
    try {
        web::json::value v = task.get();
        assert(v.is_array());
        std::cout << "The cat breeds are ";
        for (size_t i = 0; i < v.size() - 1; ++i) {
            std::cout << v[i].at("name") << ", ";
        }
        std::cout << "and " << v[v.size()-1].at("name") << ".\n";

    } catch(web::json::json_exception& e) {
        std::cerr << e.what() << "\n";
        std::abort();
    }
}

int main()
{
    web::uri_builder builder;
    builder.set_scheme("https");
    builder.set_host("api.thecatapi.com");
    builder.set_port(443);
    // This is another acceptable method to post your key, per TheCatAPI documentation:
    //builder.set_query(U("api_key"), "replace_me_with_your_api_key");
    builder.set_path("/v1/breeds");
    if (!builder.is_valid()) {
        std::cerr << "Built an invalid uri, sry!";
        return 1;
    }
    web::http::client::http_client client(builder.to_uri());

    auto request = web::http::http_request(web::http::methods::GET);
    
    // Note that though TheCatApi states that you need an API key for this /v1/breeds endpoint,
    // it doesn't actually validate the key. (Kinda not worth it, I guess)
    request.headers().add("x-api-key", "replace_me_with_your_api_key");
    auto task = client.request(request);
    task.then(task_from_response).then(print_cat_breeds_from_task).wait();
    return 0;
}