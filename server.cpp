#include <iostream>
#include <nghttp2/asio_http2_server.h>

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

int main(int argc, char *argv[]) {
  boost::system::error_code ec;
  http2 server;
  boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);
  
  // SSL register
  tls.use_private_key_file("server.key", boost::asio::ssl::context::pem);
  tls.use_certificate_chain_file("server.crt");

  configure_tls_context_easy(ec, tls); 
  
  // 使用server push只是为了提高效率，但是我要如何在加载html的时候让其加载js文件呢
  // 这里handle的意思是说在请求非“/style.css”资源的时候，我们就加载style.css的内容
  // handle pattern
  server.handle("/dash-fix.html", [](const request &req, const response &res) {
    boost::system::error_code ec;
    auto init_push = res.push(ec,"GET","/video-js.css");
    init_push->write_head(200);
    init_push->end(file_generator("video-js.css"));

    auto push = res.push(ec,"GET","/dash.all.debug.js");
    push->write_head(200);
    push->end(file_generator("dash.all.debug.js"));

    auto second_push = res.push(ec,"GET","/video.js");
    second_push->write_head(200);
    second_push->end(file_generator("video.js"));

    auto third_push = res.push(ec,"GET","/videojs-dash.min.js");
    third_push->write_head(200);
    third_push->end(file_generator("videojs-dash.min.js"));
    
    res.write_head(200);
    res.end(file_generator("dash-fix.html"));
  });


  server.handle("/index.html", [](const request &req, const response &res) {
    res.write_head(200);
    res.end(file_generator("index.html"));
  });

  server.handle("/jquery.min.js", [](const request &req, const response &res) {
     res.write_head(200);
     res.end(file_generator("jquery.min.js"));
   });
   
  server.handle("/bootstrap.min.js", [](const request &req, const response &res) {
     res.write_head(200);
     res.end(file_generator("bootstrap.min.js"));
   });


// 这里不是很清楚这样硬编码问题到底要如何解决，但是首先应该解决的是如果我不写server push 我应该如何访问我的css和js文件
  // server.handle("/dash.all.js",[](const request &req, const response &res) {
  //   res.write_head(200);
  //   res.end(file_generator("dash.all.js"));
  // });


  if (server.listen_and_serve(ec,tls,"127.0.0.1", "8080")) {
    std::cerr << "error: " << ec.message() << std::endl;
  }
}

