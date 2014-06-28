package com.RbGroup;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class RequestHandler extends Thread {

	private final static Logger logger = Logger.getLogger(RequestHandler.class.getName());
	private Socket socketConnection;
	private int currentOffset;
	
	public RequestHandler(Socket socket) {
		this.socketConnection = socket;
		
		//자동으로 초기화되나 명시적으로 써주었다.
		this.currentOffset = 0;
	}
	
	public byte[] readOffsetData() throws IOException {
		
		//Decrease Previous Reference count
		ReadData previousOffsetData = WebServer.memoryData.get(currentOffset-1);
		if (previousOffsetData != null) {
			previousOffsetData.decreaseReferenceCount();
		}
		
		//Increase Current Reference count
		ReadData currentOffsetData  = WebServer.memoryData.get(currentOffset);
		
		if (currentOffsetData  == null) {
			WebServer.fileManager.seek(currentOffset * WebServer.BUFFER_SIZE);
			byte[] bytes = new byte[WebServer.BUFFER_SIZE];
			WebServer.fileManager.read(bytes, 0, WebServer.BUFFER_SIZE);
			WebServer.memoryData.put(currentOffset, new ReadData(currentOffset, bytes));
			
			++this.currentOffset;
			return bytes;
		} else {
			currentOffsetData.IncreaseReferenceCount();
			++this.currentOffset;
			return currentOffsetData .getData();	
		}
	}
	
	@Override
	public void run() {
		logger.log(Level.INFO, "WebServer Thread Created!");
		
		OutputStream responseStream = null;
		DataOutputStream dataOutputStream = null;

		try {
			responseStream = socketConnection.getOutputStream();
			dataOutputStream = new DataOutputStream(responseStream);
			
			//
			long fileLength = WebServer.fileManager.length();
			int opperationCount = (int) (fileLength / WebServer.BUFFER_SIZE); 
			
			dataOutputStream.writeBytes("HTTP/1.1 200 Document Follows \r\n");
			dataOutputStream.writeBytes("Content-Type: application/octet-stream\r\n");
			dataOutputStream.writeBytes("Content-Disposition: attachment;filename=\"test.mp4\"\r\n");
			dataOutputStream.writeBytes("Content-Length: " + fileLength + "\r\n");
			dataOutputStream.writeBytes("\r\n");

			for (int index = 0 ; index <= opperationCount+1 ; ++index) {
				byte[] reaData = readOffsetData();
				dataOutputStream.write(reaData, 0, reaData.length);
			}
		} catch (IOException e) {
			logger.log(Level.SEVERE, "getStream Error : " + e);
		} finally {

			try {

				if (socketConnection != null) {
					socketConnection.close();
				}

				if (dataOutputStream != null) {
					dataOutputStream.close();
				}

				if (responseStream != null) {
					responseStream.close();
				}

			} catch (IOException e) {
				logger.log(Level.SEVERE, "Close Exception : " + e);
			}
		}
	}
}
