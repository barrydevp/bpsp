package main.client;

public interface Publisher {

	public void connect();

	public void pub(String topic, String content);

}
