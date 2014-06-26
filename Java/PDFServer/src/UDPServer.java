import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;

public class UDPServer {
	
	private static Logger logger = Logger.getLogger(UDPServer.class.getName()); 
	
	static String getTime() {
        SimpleDateFormat f = new SimpleDateFormat("[hh:mm:ss]");
        return f.format(new Date());
    }
	
	public static void main(String args[]) throws Exception {
		
		//UDP 서버소켓 생성, 8080포트와 연결
		DatagramSocket serverSocket = new DatagramSocket(8080);
		logger.log(Level.INFO, getTime() + " UDP 서버가 시작되었습니다.");
		
		//Data를 담을 byte배열 선언
		byte[] receiveData = new byte[1024];
		byte[] sendData = new byte[1024];
		
		//
		while (true) {
			logger.log(Level.INFO, getTime() + " 연결요청을 대기중");

			//UDP의 경우에는 각 사용자별 Packet데이터를 관리해주어야 하기 떄문에
			//DatagramPacket을 선언해야 한다.
			DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
			
			//서버패킷으로 요청된 UDP데이터를 receivePacket에 담는다.
			serverSocket.receive(receivePacket);
			InetAddress IPAddress = receivePacket.getAddress();
			int port = receivePacket.getPort();
			
			logger.log(Level.INFO, getTime() + IPAddress+":"+port+"로 부터 연결이 요청되었습니다.");
			
			String sentence = new String(receivePacket.getData());
			logger.log(Level.INFO, "RECEIVED 데이터 : " + sentence);
			String capitalizedSentence = sentence.toUpperCase();
			
			//전달받은 데이터를 대문자로 모두 변경후, echo해주기 위해 byte형태로 형변환한다
			sendData = capitalizedSentence.getBytes();
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			serverSocket.send(sendPacket);
		}
	}
}