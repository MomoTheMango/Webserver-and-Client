#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// 클라이언트 요청을 처리하는 함수
void handle_request(tcp::socket& socket) {
	try {

		// 클라리언트 데이터를 받기
		char data[1024];
		size_t length = socket.read_some(boost::asio::buffer(data));
	
		// 요청 문자열로 출력
		std::string request(data, length);
		std::cout << "Received request: " << request << '\n';

		// 기본 응답 설정
		std::string response; 
		std::string name = "Unknown";
		std::string age = "Unknown";
		std::string hobby = "Unknown";
		
		// GET 요청 처리
		if (request.find("GET") == 0) {
			std::string name_param = "name=";
			std::string age_param = "age=";
			std::string hobby_param = "hobby=";
			
			// 파라미터 찾기
			size_t name_pos = request.find(name_param);
			size_t age_pos = request.find(age_param);
			size_t hobby_pos = request.find(hobby_param);
			
			// name 파라미터 위치 확인
			if (name_pos != std::string::npos) {
				size_t start_pos = name_pos + name_param.length();
				size_t end_pos = request.find_first_of("& ", start_pos);
				if (end_pos == std::string::npos) {
					end_pos = request.length();
				}
				name = request.substr(start_pos, end_pos - start_pos);
			}

			// age 파라미터 위치 확인
			if (age_pos != std::string::npos) {
				size_t start_pos = age_pos + age_param.length();
				size_t end_pos = request.find_first_of("& ", start_pos);
				if (end_pos == std::string::npos) {
					end_pos = request.length();
				}
				age = request.substr(start_pos, end_pos - start_pos);
			}

			// hobby 파라미터 위치 확인
			if (hobby_pos != std::string::npos) {
				size_t start_pos = hobby_pos + hobby_param.length();
				size_t end_pos = request.find_first_of("& ", start_pos);
				if (end_pos == std::string::npos) {
					end_pos = request.length();
				}
				hobby = request.substr(start_pos, end_pos - start_pos);
			}

			// 응담 생성
			response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nName: " + name + "\nAge: " + age + "\nHobby: " + hobby + '\n';
			
		}

		// 클라이언트에 응답 전송
		boost::asio::write(socket, boost::asio::buffer(response));
	
	}
	catch (const std::exception& e) {
		std::cerr << "Error handling request: " << e.what() << '\n';
	}
}

int main() {

	try {

		// io_context 객체 생성
		boost::asio::io_context io_context;

		// 서버 주소와 포트 설정
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
		std::cout << "Server is running on port 8080..." << '\n';

		// 서버가 계속 클라이언트의 연결을 기다리며 요청 처리
		while (1) {
			// 소켓 생성
			tcp::socket socket(io_context);
			// 클라이언트 연결 수락
			acceptor.accept(socket);
			// 요청 처리
			handle_request(socket);
			// 소켓 닫기
			socket.close();
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Server Failed" << e.what() << '\n';
	}

	return 0;

}