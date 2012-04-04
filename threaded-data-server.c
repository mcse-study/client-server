#include <gio/gio.h>
#include <glib.h>
#include <gst/gst.h>

static gboolean on_run (GThreadedSocketService * service,
                        GSocketConnection      * connection,
                        GObject                * source_object,
                        gpointer                 user_data)
{
  GSocketAddress *socket_address = NULL;
  GInetAddress   *address        = NULL;
  guint16         port           = 0;

  GThread        *self           = NULL;

  GSocket    *socket        = NULL;
  gchar       buf[1024];
  gsize       bytes_written = 0;
  GError     *local_error   = NULL;

  socket_address = g_socket_connection_get_remote_address (connection, NULL);

  address = g_inet_socket_address_get_address (
      G_INET_SOCKET_ADDRESS (socket_address));

  port = g_inet_socket_address_get_port (
      G_INET_SOCKET_ADDRESS (socket_address));

  self = g_thread_self ();

  g_debug ("Thread %p is handling new Connection from %s:%d",
      self, g_inet_address_to_string (address), port);

  socket  = g_socket_connection_get_socket (connection);
  
  while (1)
  {
    bytes_written = g_socket_send (
        socket, buf, sizeof (buf), NULL, &local_error);
    if (-1 == bytes_written)
    {
    	g_warning ("Failed to send: %s", local_error->message);
    	g_clear_error (&local_error);
    	break;
    }
    
    g_usleep (500000);
  }

  return TRUE;
}

int main (int argc, char * argv[])
{
  GMainLoop      *loop           = NULL;
  GInetAddress   *address        = NULL;
  GSocketAddress *socket_address = NULL;
  GSocketService *service        = NULL;
  GError         *local_error    = NULL;
  
  g_type_init ();

  address = g_inet_address_new_from_string("127.0.0.1");

  socket_address = g_inet_socket_address_new (address, 8080);

  service = g_threaded_socket_service_new (3);

  g_signal_connect (G_OBJECT (service),
                    "run",
                    G_CALLBACK (on_run),
                    NULL);

  g_socket_listener_add_address (G_SOCKET_LISTENER (service),
                                 socket_address,
                                 G_SOCKET_TYPE_STREAM,
                                 G_SOCKET_PROTOCOL_TCP,
                                 NULL,
                                 NULL,
                                 &local_error);
  if (local_error)
  {
    g_warning ("Failed to add address: %s", local_error->message);
    return 0;
  }

  g_object_unref (socket_address);
  
  g_object_unref (address);

  g_socket_service_start (service);
  
  loop = g_main_loop_new (NULL, FALSE);
  
  g_main_loop_run (loop);

  g_main_loop_unref (loop);

  g_socket_service_stop (service);

  return 0;
}

