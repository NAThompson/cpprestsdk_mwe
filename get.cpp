#include <iostream>
#include <cpprest/http_client.h>
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

void print_joke_from_task(pplx::task<web::json::value> task)
{   
    web::json::value v = task.get();
    std::cout << v.at("value") << "\n";
}

int main()
{
    web::http::client::http_client client(U("https://api.chucknorris.io"));
    auto request = client.request(web::http::methods::GET, "/jokes/random");
    request.then(task_from_response).then(print_joke_from_task).wait();
    return 0;
}