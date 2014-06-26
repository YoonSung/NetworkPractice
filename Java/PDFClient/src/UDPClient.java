import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.logging.Level;
import java.util.logging.Logger;

public class UDPClient {
	
	private static Logger logger = Logger.getLogger(UDPClient.class.getName());
	
	public static void main(String args[]) throws Exception {
		//키보드 입력을 받기위한 선언
		BufferedReader inFromUser = new BufferedReader(new InputStreamReader(System.in));
		
		//UDP 클라이언트 소켓 생성
		DatagramSocket clientSocket = new DatagramSocket();
		
		//InetAddress를 Host name을 통해서 가져온다.
		InetAddress IPAddress = InetAddress.getByName("localhost");
		
		//Data를 담을 byte배열 선언		
		byte[] sendData = new byte[1024];
		byte[] receiveData = new byte[1024];
		
		//User 키보드로부터 전달받은 String 데이터
		String sentence = inFromUser.readLine();
		
		//Packet전송을 위해 byte형태로 가져온다.
		sendData = sentence.getBytes();
		
		//UDP의 경우에는 각 사용자별 Packet데이터 전송이 따로 이루어지므로,
		//DatagramPacket을 선언해야 한다.
		//만약 큰값을 선언하면..? Layer 하단부에서 잘게 잘라서 전송하는가??
		DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, 8080);
		
		//UDP Client소켓을 통행 Packet데이터를 전송한다.
		clientSocket.send(sendPacket);
		
		//전달받는 데이터를 담을 Packet객체를 선언
		DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
		
		//Server로부터 받은 데이터를 UDP Packet객체에 저장한다.
		clientSocket.receive(receivePacket);
		
		//Server로부터 받은 String 데이터
		String modifiedSentence = new String(receivePacket.getData());
		logger.log(Level.INFO, "From Server 데이터 : " + modifiedSentence);
		clientSocket.close();
	}
}