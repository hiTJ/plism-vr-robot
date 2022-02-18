#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
 
int main() {
	int sockfd;
	int client_sockfd;
	struct sockaddr_in addr;
 
	socklen_t len = sizeof( struct sockaddr_in );
	struct sockaddr_in from_addr;
 
	char buf[1024];
 
	// 受信バッファ初期化
	memset( buf, 0, sizeof( buf ) );
 
	// ソケット生成
	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		perror( "socket" );
	}
 
	// 待ち受け用IP・ポート番号設定
	addr.sin_family = AF_INET;
	addr.sin_port = htons( 51000 );
	// ローカルホストを指定
	addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	//addr.sin_addr.s_addr = INADDR_ANY;
 
	// バインド
	if( bind( sockfd, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 ) {
		perror( "bind" );
	}
 
	// 受信待ち
	if( listen( sockfd, SOMAXCONN ) < 0 ) {
		perror( "listen" );
	}
 
	// クライアントからのコネクト要求待ち
	if( ( client_sockfd = accept( sockfd, (struct sockaddr *)&from_addr, &len ) ) < 0 ) {
		perror( "accept" );
	}
 
	// 受信
	int rsize;
	while( 1 ) {

		rsize = recv( client_sockfd, buf, sizeof( buf ), 0 );
		//printf( "rsize = %d\n", rsize );

		if ( rsize > 0 ) {
			printf( "receive: %s\n", buf );
		} else if ( rsize == 0) {
		break;
	}
	}
 
	// ソケットクローズ
	close( client_sockfd );
	close( sockfd );
 
	return 0;
}
