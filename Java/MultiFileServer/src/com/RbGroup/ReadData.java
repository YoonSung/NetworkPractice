package com.RbGroup;

public class ReadData {
	
	private int sequence;
	private int reference;
	private byte[] data;
	
	public ReadData(int sequence, byte[] data) {
		this.sequence = sequence;
		this.data = data;
		this.reference = 1;
	}
	
	public int getReferenceCount() {
		return reference;
	}
	public synchronized void IncreaseReferenceCount() {
		++this.reference;
	}
	
	public synchronized void decreaseReferenceCount() {
		--this.reference;
		
		if ( reference == 0 )
			WebServer.removeReadDataFromMemory(sequence);
	}
	
	public byte[] getData() {
		return data;
	}
	public void setData(byte[] data) {
		this.data = data;
	}
}
