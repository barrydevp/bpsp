package com.utils;

public class SystemUtils {
    public final static void clearConsole() {
        try {
            final String os = System.getProperty("os.name");
            if (os.contains("Windows")) {
                Runtime.getRuntime().exec("cls");
            } else {
                Runtime.getRuntime().exec("clear");
            }
        } catch (final Exception e) {
            e.printStackTrace();
        }

        // System.out.print("\033[H\033[2J");
        // System.out.flush();
    }

    public static java.net.URL getImagePath(String file) {
        return SystemUtils.class.getClassLoader().getResource("images/" + file);
    }
}
