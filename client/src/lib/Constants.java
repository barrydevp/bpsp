package lib;

public final class Constants {
	//**general */
	public static final int FIXED_HEADER_SIZE = 8;
	public static final String DEFAULT_SERVER_IP_ADDR = "0.0.0.0";
	public static final int DEFAULT_SERVER_PORT = 29010;
	
	//**frame */
	public static final int OPCODE_OFFSET = 2;
	public static final int FLAG_OFFSET = 3;
	private Constants() {} // Prevents instantiation
}
