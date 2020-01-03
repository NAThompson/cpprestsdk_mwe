#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/http_headers.h>
#include <cpprest/uri_builder.h>
#include <cpprest/json.h>

void task_from_response(web::http::http_response response)
{
    if (response.status_code() == web::http::status_codes::OK)
    {
        return;
    }
    else if (response.status_code() == web::http::status_codes::Unauthorized)
    {
        std::cerr << "Server rejected request with 'Unauthorized' status code; "
                  << "have you requested an API key from https://thecatapi.com/signup, "
                  << "and is your API key correct?\n";
        std::abort();
    }
    std::cerr << "Returned " << response.status_code() << ": " << response.reason_phrase() << "\n";
    std::abort();
}

int main()
{
    web::uri_builder builder;
    builder.set_scheme("https");
    builder.set_host("api.thecatapi.com");
    builder.set_port(443);
    // This is another acceptable method to post your key, per TheCatAPI documentation:
    //builder.set_query(U("api_key"), "replace_me_with_your_api_key");
    builder.set_path("/v1/images/upload");
    if (!builder.is_valid()) {
        std::cerr << "Built an invalid uri, sry!";
        return 1;
    }
    web::http::client::http_client client(builder.to_uri());

    auto request = web::http::http_request(web::http::methods::POST);

    request.headers().add("x-api-key", "replace_me_with_your_api_key");
    //request.headers().add("Content-Type", "multipart/form-data");
    request.headers().add("Content-Type","application/octet-stream");
    std::vector<unsigned char> v;
    auto task = client.request(request);
    task.then(task_from_response).wait();
    return 0;
}