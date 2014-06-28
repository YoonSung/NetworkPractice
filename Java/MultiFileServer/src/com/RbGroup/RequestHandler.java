package com.RbGroup;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class RequestHandler extends Thread {

	private final static Logger logger = Logger.getLogger(RequestHandler.class
			.getName());
	Socket socketConnection;

	public RequestHandler(Socket socket) {
		this.socketConnection = socket;
	}

	@Override
	public void run() {
		logger.log(Level.INFO, "WebServer Thread Created!");

		InputStream requestStream = null;
		// inputStream을 효과적으로 사용하기 위한 Wrapper클래스.
		// 일반적인 inputStream은 한바이트씩 처리하나, inputStreamReader는
		// chunk형태로 처리가 가능하다.
		// 이렇게 wrapper클래스를 활용하는 패턴을 decorate Pattern이라고 한다.
		InputStreamReader inputStreamReader = null;
		
		OutputStream responseStream = null;
		DataOutputStream dataOutputStream = null;

		try {
			requestStream = socketConnection.getInputStream();
			responseStream = socketConnection.getOutputStream();

			inputStreamReader = new InputStreamReader(requestStream);
			dataOutputStream = new DataOutputStream(responseStream);
			
			// FilePath
			String filePath = "/Users/next/Desktop/test.mp4";
			File file = new File(filePath);

			if (file.exists()) {

				 dataOutputStream.writeBytes("HTTP/1.1 200 Document Follows \r\n");
				 dataOutputStream.writeBytes("Content-Type: application/octet-stream\r\n");
				 dataOutputStream.writeBytes("Content-Disposition: attachment;filename=\"test.mp4\"\r\n");
				 dataOutputStream.writeBytes("Content-Length: " + file.length() + "\r\n");
				 dataOutputStream.writeBytes("\r\n");

				DataInputStream dataInputStream = new DataInputStream(new FileInputStream(file));
				byte[] arr = new byte[4096];
				int streamLength = 0;
				while ((streamLength = dataInputStream.read(arr)) != -1) {
					// send the first len bytes of arr over socket.
					dataOutputStream.write(arr, 0, streamLength);
				}
			}
		} catch (IOException e) {
			logger.log(Level.SEVERE, "getStream Error : " + e);
		} finally {

			try {

				if (socketConnection != null) {
					socketConnection.close();
				}

				if (inputStreamReader != null) {
					inputStreamReader.close();
				}

				if (dataOutputStream != null) {
					dataOutputStream.close();
				}

				if (responseStream != null) {
					responseStream.close();
				}

				if (requestStream != null) {
					requestStream.close();
				}

			} catch (IOException e) {
				logger.log(Level.SEVERE, "Close Exception : " + e);
			}
		}
	}
}
