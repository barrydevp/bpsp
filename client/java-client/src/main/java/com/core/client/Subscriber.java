package com.core.client;

public interface Subscriber {

	public void connect();

	public void sub(String topic);

	public void unSub(String topic);

}
