#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// �����κ��� ���� ������ ó���ϴ� �Լ�
void read_response(tcp::socket& socket) {

	try {

		// ������ ���� ���� ����
		boost::asio::streambuf response_buffer;

		// ���� ��� ������ �����͸� ����
		boost::asio::read_until(socket, response_buffer, "\r\n\r\n");

		// ���� ��� ���ڿ��� ��ȯ
		std::string header_string(boost::asio::buffers_begin(response_buffer.data()), boost::asio::buffers_begin(response_buffer.data()) + response_buffer.size());
		
		// ���� ���� ��� ũ�⸸ŭ �Һ�
		response_buffer.consume(response_buffer.size());
		
		// ��� ��(\r\n\r\n) ������ Json ���
		std::size_t header_end = header_string.find("\r\n\r\n");
		if (header_end != std::string::npos) {
			header_string.erase(0, header_end + 4);
			std::cout << header_string;
		}

		// �����ִ� Json ���
		std::string line;
		while (boost::asio::read(socket, response_buffer, boost::asio::transfer_at_least(1))) {
			std::cout << &response_buffer;
		}

	}
	catch (std::exception& e) {
		std::cerr << '\n' << e.what() << "\n\n";
	}
}

// ������ HTTP ��û�� ������ �Լ�
void send_request(boost::asio::io_context& ioc, const std::string& host, const std::string& port, const std::string& target) {

	try {

		// ȣ��Ʈ ���� Ȯ��
		tcp::resolver resolver(ioc);
		auto endpoints = resolver.resolve(host, port);
		
		// ���� ���� �� ����
		tcp::socket socket(ioc);
		boost::asio::connect(socket, endpoints);
		
		// HTTP GET ��û �޼��� ����
		std::string request = "GET " + target + " HTTP/1.1\r\n";
		request += "Host: " + host + "\r\n";
		request += "Connection: close\r\n";
		request += "\r\n";

		// ��û �޼��� ����
		boost::asio::write(socket, boost::asio::buffer(request));

		// ���� ó�� �Լ� ȣ��
		read_response(socket);

		// ���� �ݱ�
		socket.close();

	}
	catch (std::exception& e) {
		std::cerr << '\n' << e.what() << "\n\n";
	}

}

int main() {

	// io_context ��ü ����
	boost::asio::io_context ioc;
	// ��û�� ���� ����
	std::string host = "jsonplaceholder.typicode.com";
	std::string port = "80";
	std::string id = "1";

	// ����ڰ� '0'�� �Է��Ҷ����� ���������� ��û ����
	while (id != "0") {

		// ��û�� ���
		std::string target = "/posts/";
		
		// ����ڷκ��� ����Ʈ ID �Է¹ޱ�
		std::cout << "Please input post ID (1~ 100, 0 to exit): ";
		std::cin >> id;

		// ��û ��� ������Ʈ
		target += id;

		// ��û ������
		send_request(ioc, host, port, target);
	}

	return 0;

}