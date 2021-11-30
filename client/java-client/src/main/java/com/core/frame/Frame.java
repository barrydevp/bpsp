package com.core.frame;

import com.resources.Constants;

import java.nio.ByteBuffer;

public class Frame {
	private FrameFixedHeader fixedHeader = new FrameFixedHeader(); // fixed header attribute

	//**data attributes */
	private String varHeaders = "";
	private String data = "";

	//**constructors */
	public Frame() {}
	public Frame(byte opcode) {
		this.fixedHeader.setOpcode(opcode);
	}
	public Frame(FrameFixedHeader fixedHeader, String varHeaders, String data) throws Exception {
		if (fixedHeader.getVarsHeaderSize() != varHeaders.length() || fixedHeader.getDataSize() != data.length()) {
			throw new Exception("var header size or data size is not valid");
		}
		this.fixedHeader = fixedHeader;
		this.varHeaders = varHeaders;
		this.data = data;
	}
	public Frame(byte opcode, String varHeaders, String data) {
		try {
			this.fixedHeader.setOpcode(opcode);
			this.fixedHeader.setVarsHeaderSize((short)varHeaders.length());
			this.data = data;
		} catch (Exception e) {
			System.out.println(e);
		}
		this.varHeaders = varHeaders;
		this.data = data;
	}
	public Frame(byte opcode, byte flag, String varHeaders, String data) {
		try {
			this.fixedHeader = new FrameFixedHeader((short)varHeaders.length(),opcode,flag,data.length());
		} catch (Exception e) {
			System.out.println(e);
		}
		this.varHeaders = varHeaders;
		this.data = data;
	}
	public Frame(short varsHeaderSize, byte opcode, byte flag, int dataSize, String varHeaders, String data) throws Exception {
		if (varsHeaderSize != (short)varHeaders.length()) {
			throw new ExceptionInInitializerError("size of variable header is not correct");
		}
		if (dataSize != data.length()) {
			throw new ExceptionInInitializerError("size of data is not correct");
		}
		this.fixedHeader = new FrameFixedHeader((short)varHeaders.length(),opcode,flag,data.length());
		this.varHeaders = varHeaders;
		this.data = data;
	}

	//**get & set methods */
	public FrameFixedHeader getFixedHeader() {
		return fixedHeader;
	}
	public String getVarHeaders() {
		return varHeaders;
	}
	public String getData() {
		return data;
	}
	public int getFrameSize() {
		int frameSize = Constants.FIXED_HEADER_SIZE + (int)fixedHeader.getVarsHeaderSize() + (int)fixedHeader.getDataSize();
		return frameSize;
	}
	public void setVarHeaders(String varHeaders) {
		this.varHeaders = varHeaders;
		this.fixedHeader.setVarsHeaderSize((short)varHeaders.length());
	}
	public void setVarHeader(String key, String value) {
		this.varHeaders = this.varHeaders.concat("\"" + key + "\"\"" + value + "\";");
		this.fixedHeader.setVarsHeaderSize((short)varHeaders.length());
	}
	public void setData(String data) {
		this.data = data;
		this.fixedHeader.setDataSize(data.length());
	}
	public void setFrameControl(byte opcode,byte flag) {
		this.fixedHeader.setOpcode(opcode);
		this.fixedHeader.setFlag(flag);
	}
	public void setFixedHeader(FrameFixedHeader fixedHeader) {
		this.fixedHeader = fixedHeader;
	}

	//**behavior methods */
	public void print() { // print detail of frame to console
		System.out.println("---------[FRAME]---------");
		fixedHeader.print();
		if ((int)fixedHeader.getVarsHeaderSize() > 0) {
			System.out.println(varHeaders);
		}
		if (fixedHeader.getDataSize() > 0) {
			System.out.println("##########");
			System.out.println(data);
		}
	}

	//**transform methods */
	public byte[] toByteArray() {
		int frameSize = getFrameSize();

		ByteBuffer buffer = ByteBuffer.allocate(frameSize);

		buffer.put(fixedHeader.toByteArray());
		buffer.put(varHeaders.getBytes());
		buffer.put(data.getBytes());

		return buffer.array();
	}
}
