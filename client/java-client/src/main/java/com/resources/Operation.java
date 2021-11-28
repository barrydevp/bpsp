package com.resources;

public enum Operation {
	INFO("INFO", (short)1),
	CONNECT("CONNECT", (short)2),
	PUB("PUB",(short)3),
	SUB("SUB",(short)4),
	UNSUB("UNSUB",(short)5),
	MSG("MSG",(short)6),
	OK("OK",(short)7),
	ERR("ERR",(short)8)
	;

	private final String text;
	private final short code;

	Operation(final String text, final short code) {
		this.text = text;
		this.code = code;
	}

	public String getText() {
			return text;
	}

	public short getCode() {
		return code;
	}
}
