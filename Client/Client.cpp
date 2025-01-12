#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// 서버로부터 받은 응답을 처리하는 함수
void read_response(tcp::socket& socket) {

	try {

		// 응답을 받을 버퍼 생성
		boost::asio::streambuf response_buffer;

		// 응답 헤더 끝까지 데이터를 읽음
		boost::asio::read_until(socket, response_buffer, "\r\n\r\n");

		// 응답 헤더 문자열로 변환
		std::string header_string(boost::asio::buffers_begin(response_buffer.data()), boost::asio::buffers_begin(response_buffer.data()) + response_buffer.size());
		
		// 응답 버퍼 헤더 크기만큼 소비
		response_buffer.consume(response_buffer.size());
		
		// 헤더 끝(\r\n\r\n) 이후의 Json 출력
		std::size_t header_end = header_string.find("\r\n\r\n");
		if (header_end != std::string::npos) {
			header_string.erase(0, header_end + 4);
			std::cout << header_string;
		}

		// 남아있는 Json 출력
		std::string line;
		while (boost::asio::read(socket, response_buffer, boost::asio::transfer_at_least(1))) {
			std::cout << &response_buffer;
		}

	}
	catch (std::exception& e) {
		std::cerr << '\n' << e.what() << "\n\n";
	}
}

// 서버로 HTTP 요청을 보내는 함수
void send_request(boost::asio::io_context& ioc, const std::string& host, const std::string& port, const std::string& target) {

	try {

		// 호스트 정보 확보
		tcp::resolver resolver(ioc);
		auto endpoints = resolver.resolve(host, port);
		
		// 소켓 생성 및 연결
		tcp::socket socket(ioc);
		boost::asio::connect(socket, endpoints);
		
		// HTTP GET 요청 메세지 생성
		std::string request = "GET " + target + " HTTP/1.1\r\n";
		request += "Host: " + host + "\r\n";
		request += "Connection: close\r\n";
		request += "\r\n";

		// 요청 메세지 전송
		boost::asio::write(socket, boost::asio::buffer(request));

		// 응답 처리 함수 호출
		read_response(socket);

		// 소켓 닫기
		socket.close();

	}
	catch (std::exception& e) {
		std::cerr << '\n' << e.what() << "\n\n";
	}

}

int main() {

	// io_context 객체 생성
	boost::asio::io_context ioc;
	// 요청할 서버 정보
	std::string host = "jsonplaceholder.typicode.com";
	std::string port = "80";
	std::string id = "1";

	// 사용자가 '0'을 입력할때까지 지속적으로 요청 가능
	while (id != "0") {

		// 요청할 경로
		std::string target = "/posts/";
		
		// 사용자로부터 포스트 ID 입력받기
		std::cout << "Please input post ID (1~ 100, 0 to exit): ";
		std::cin >> id;

		// 요청 경로 업데이트
		target += id;

		// 요청 보내기
		send_request(ioc, host, port, target);
	}

	return 0;

}