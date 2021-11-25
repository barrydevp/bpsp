package lib;

import java.nio.ByteBuffer;
import lib.Constants;

public class Frame {
	private short varsHeaderSize;
	private byte opcode;
	private byte flag;
	private int dataSize;
	private String varHeaders;
	private String data;

	public Frame(short varsHeaderSize, byte opcode, byte flag, int dataSize, String varHeaders, String data) {
		this.varsHeaderSize = varsHeaderSize;
		this.opcode = opcode;
		this.flag = flag;
		this.dataSize = dataSize;
		this.varHeaders = varHeaders;
		this.data = data;
	}

	public byte[] toByteArray() {
		ByteBuffer buffer = ByteBuffer.allocate(Constants.FIXED_HEADER_SIZE + varsHeaderSize + dataSize);

		buffer.putShort(varsHeaderSize);
		buffer.put(opcode);
		buffer.put(flag);
		buffer.putInt(dataSize);
		buffer.put(varHeaders.getBytes());
		buffer.put(data.getBytes());

		return buffer.array();
	}
}
