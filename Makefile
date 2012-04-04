all:
	gcc -o data-server data-server.c `pkg-config --cflags --libs glib-2.0 gio-2.0 gstreamer-0.10`
	gcc -o threaded-data-server threaded-data-server.c `pkg-config --cflags --libs glib-2.0 gio-2.0 gstreamer-0.10`

run_server:
	./data-server

run_threaded_server:
	./threaded-data-server

run_client:
	gst-launch tcpclientsrc host=localhost port=8080 protocol=none ! fakesink dump=true

clean:
	rm -f data-server threaded-data-server
