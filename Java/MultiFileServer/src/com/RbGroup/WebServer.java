package com.RbGroup;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.net.ServerSocket;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;

public class WebServer {
	
	private final static Logger logger = Logger.getLogger(WebServer.class.getName());
	private final static String filePath = "/Users/next/Desktop/test.mp4";
	protected final static int BUFFER_SIZE = 1024;
	
	protected static HashMap<Integer, ReadData> memoryData = new HashMap<Integer, ReadData>();
	protected static RandomAccessFile fileManager;
	
	protected static void removeReadDataFromMemory(int offset) {
		memoryData.remove(offset);
	}
	
	private static String getTime() {
		SimpleDateFormat simpleDateFormat = new SimpleDateFormat("[hh:mm:ss]");
		return simpleDateFormat.format(new Date());
	}
	
	public static void main(String[] args) {
        try {
        	// 서버소켓을 생성한다. 웹서버는 기본적으로 80번 포트를 사용한다.
			ServerSocket listenSocket = new ServerSocket(3000);
			logger.log(Level.INFO, "WebServer Socket Created");
			
			// 파일접근을 위한 객체를 생성한다.
			fileManager = new RandomAccessFile(filePath, "r");
			
			
	        // 클라이언트가 연결될때까지 대기한다.
	        Socket socket;
	        while ((socket = listenSocket.accept()) != null) {
	        	logger.log(Level.INFO, getTime() + socket.getInetAddress());
	        	RequestHandler requestHandler = new RequestHandler(socket);
	            requestHandler.start();
	        }
			
	        if(listenSocket !=null) {
	        	logger.log(Level.INFO, "Socket close");
	        	listenSocket.close();
	        }
		} catch (IOException e) {
			logger.log(Level.SEVERE, "Create WebServer Error : " + e);
		}
	}
}
