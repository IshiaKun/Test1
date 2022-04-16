//������ �����Ǹ� ������ ������ �մϴ�!
//�����Ƿ� ������ �ϴ� ������?
//�ܺο��� ������ �� ������ �ۺ���IP�� �ʿ�������, ������ �� ������
//���� ���������� ����IP�� �� �ſ���! ��� �̾߱��� �ʿ䰡 �ֽ��ϴ�!
//���� IP�� ���⿡�ٰ� �Է����ֽø� �˴ϴ�!
#define SERVER_PRIVATE_IP "172.31.41.59"

//��ǻ�Ϳ��� ���ÿ� �������� ���α׷��� �۵��ϰ� �ֽ��ϴ�!
//���縵�� �ϰ� �־����! ��Ʈ��ũ�� ����ϰ� ����!
//�������� ���� �ѵ׽��ϴ�! �����Ǹ� ������ ��ȭ�� �õ��ϸ��! ���� �޽����� ���縵 ������, ������ ������ ���� �� ���� �����!
//"��Ʈ"��� �ϴ� ���� ���� �޽������� ������ �� �ְ� �����!
//�� �� ��Ʈ�� �ָ� �� ���α׷��� �ٰԿ�^^ ��� �ϴ� ����!
//49152 ~ 65535 �� �����Ӱ� ����� �� �ִ� "���� ��Ʈ"�ϱ� �� ���̿� �ִ� ������ �������ٰԿ�!
#define SERVER_PORT 55123

//���������� �޽����� ���� �̴ϴ�!
//�޽��� ������ ���� ���� �����! ��Ʈ��ũ �������� ���������� �ִµ� �������� �Ѱ谡 ������ ���ۿ� ����!
//���� �׷��Ա��� ū �޽����� ���� ������ ���� �Ŵϱ� ������ �ִ� ���ۻ���� �������ֵ��� �սô�!
#define MAX_BUFFER_SIZE 1024

//�ִ밪�� ���ؾ��ϴ� �ٸ� ���� ���� �ſ���!
#define MAX_USER_NUMBER 100

#include <iostream>

//���� �����쿡�� �ȳ��ɴϴ�! ���������� ����� ����� �帮�� �ִ� �ſ���!
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

using namespace std;

//poll�̶�� �ϴ� ���� ������ ������ ���־��� ��! �ν��ؼ� �����ִ� �����̿���!
//������ ������ �޽����� �������� ��! ���� ������ ���ư�����!
//FD�� ������? File Descripter ������ �������ִ� �༮�̿�?
//������ ����ȭ�� ���ε�! �������� �����̶�� �ϴ� �͵� ������ ���� ���·� ������ �մϴ�!
//���� ��ȣ�� ������ �� �ִ� ������ �� �ſ���! ���Ͽ� �ش�Ǵ� ���� ����!
struct pollfd pollFDArray[MAX_USER_NUMBER];


int StartServer(int currentFD);

int main()
{
	//��⸸ �ϴ� fd�� �ʿ��ؿ�!
	//��� �̻��� �ְ� �ͼ� �̾߱⸦ �ϸ� �ȵ��߰���!
	//��ǻ�ʹ� ó�� ���� IP�� �ͼ� �̾߱��ϸ� "��ȭ �߸��ż̾��!" ��� �ϸ鼭 ���������!
	//ListenFD��� �ϴ� �ִ� �� ����� �̴ϴ�!
	//����! �� ���� ���� ������ �ϰ� �����Ű� ���ϴ�!
	//�ٸ� FD���� ���ο� ������ �˷��ִ� ���ҷ� �� �ſ���! (�Ա� ������ �ϴ� �ſ���!)
	//0��° ������ ������������ ����� �̴ϴ�!
	struct pollfd& ListenFD = pollFDArray[0];

	//���� ���� ��
	unsigned int currentUserNumber = 0;

	              //IPv4(4����Ʈ¥�� IP)
	ListenFD.fd = socket(AF_INET, SOCK_STREAM, 0);
	ListenFD.events = POLLIN;
	ListenFD.revents = 0;

	//     0�� ���������̴ϱ�!
	for (int i = 1; i < MAX_USER_NUMBER; i++)
	{
		//File Descripter�� ��ϵ��� ���� pollFD�¿�! ������ �� �� ������!
		//�ش�Ǵ� ������ ���ٴ� ���Դϴ�!
		pollFDArray[i].fd = -1;
	};

	//���⼭ FD�� �غ� �Ǿ���! ������ �������ô�!
	//���� ������ ������ �������ָ鼭 ������ ������ �ſ���!
	StartServer(ListenFD.fd);

	while (true)
	{
		//poll�� ���ؼ� ������ �帱 ��! ������ ������ �޽����� �������� �� ����!
		//0�������� ���� �־ ���� ���Ͽ� ����� ���� ������ �� �� �ְ� ������ ����������!
		int result = poll(pollFDArray, MAX_USER_NUMBER, -1);

		//������ �õ��ϰ� �;��ϴ� ������ ���� �غ��մϴ�! ���ο� ������ ����� ���� ����?
		struct sockaddr_in connectSocket;
		//�����ϰ��� �ϴ� ������ �ּ� ������!
		socklen_t addressSize;

		//���� �θ�������? 0�̸� �ƹ��� �����ߴ�! 15��� �ϸ�, 15���� �θ���!
		if (result > 0)
		{
			//���� ���Ͽ� ���� Ȯ��!
			//������ ������ �õ��ϰ� �ֽ��ϴ�!
			if (ListenFD.revents == POLLIN)
			{
				cout << "Someone Connected!" << endl;
				//����� ������! �����¿� ������ ������ �ʹٸ� ��� �����ǰ� �ƴ϶�� �Ѵٸ��.. �� �޾���� �մϴ�!
				int currentFD = accept(ListenFD.fd, (struct sockaddr*)&connectSocket, &addressSize);

				//0���� ���� �������� ���� �־ ��ü ���� ���� -1�� ���¿��� ���ҰԿ�!
				if (currentUserNumber < MAX_USER_NUMBER - 1)
				{
					//0�� ���� �����̴ϱ�! 1���� �����ؼ� ���� ������!
					for (int i = 1; i < MAX_USER_NUMBER; i++)
					{
						//����ִ� pollFD�� ã�� �ſ���!
						if (pollFDArray[i].fd == -1)
						{
							//���� ������ ������ File Descriptor�� �޾ƿ���!
							pollFDArray[i].fd = currentFD;
							pollFDArray[i].events = POLLIN;
							pollFDArray[i].revents = 0;

							cout << "Connected : " << i << endl;

							//���� �� �� �߰���!
							++currentUserNumber;
							//�� �� �����ϱ�! �����!
							break;
						};
					};
				};
			};
		};
	};

	//���� ���� �ݰ�
	close(ListenFD.fd);

	for (int i = 0; i < MAX_USER_NUMBER; i++)
	{
		//��.. ���� �־�? �ݾ�!
		if (pollFDArray[i].fd != -1) close(pollFDArray[i].fd);
	};
	return -4;
}

int StartServer(int currentFD)
{
	//������ �����Ϸ��� �ϴµ�.. �ƴ�!
	//���� ������ �����־��!
	if (currentFD == -1)
	{
		//���϶����� ������ �����!
		perror("socket()");
		//Ȥ�� �𸣴ϱ� ���� ���ֱ�!
		close(currentFD);
		return -1;
	};

	//���Ͽ��ٰ� INET�̶�� ������ �־���µ���
	//�׷�.. 4����Ʈ¥�� IP�� ���� �˰ڴµ�.. �׷��� IP�� ����?
	//�׷��� ����� ������ IP�� ���� ���Ͽ��ٰ� ����� ���־�� �մϴ�!
	sockaddr_in address;

	//Ȥ�� �𸣴ϱ� ���� �ʱ�ȭ �س��� ������ �����Կ�!
	//address ���θ� ���� 0���� ���߱�!
	memset(&address, 0, sizeof(address));

	//�����̶� ������ ������ �����ֵ��� �ҰԿ�!
	address.sin_family = AF_INET;
	//���� ���� IP�� ���⿡�� �־��ֵ��� �սô�!
	address.sin_addr.s_addr = inet_addr(SERVER_PRIVATE_IP);
	//��Ʈ���� �����־�� �߾���?
	address.sin_port = htons(SERVER_PORT);

	//�ּҰ� ���⿡�� �� �����ٸ� ������ ������ �ȵ� �ſ���! ���常 �� ������!
	//����ؼ� ������ ������ �� �Ŵϱ�!
	//���� ������ �ּҸ� ���Ͽ��ٰ� "����" �� �ſ���!                 ���д�!
	if (bind(currentFD, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		perror("bind()");
		close(currentFD);
		return -1;
	};

	//������ ���� ������ �۵��� ���Ѻ��ϴ�!
	//�׷� ��¥�� ������ ���ư��� �Ű���?
	if (listen(currentFD, 8) == -1)
	{
		perror("listen()");
		close(currentFD);
		return -1;
	};

	cout << "Server is On the way" << endl;

	//����� ��� �÷��� �Ǹ��ϰ� �̰ܳ��̽��ϴ�
	return 1;
}