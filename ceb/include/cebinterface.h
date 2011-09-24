#ifndef CEBINTERFACE_H
#define CEBINTERFACE_H

class QMainWindow;

class CeBInterface
 {
 public:
     virtual ~CeBInterface() {}

     /** Init the plugin.
      * Must be called before using getMainWindow().
      */
     virtual void init() const = 0;
     /** De-init the plugin.
      * Must be called when plugin is no more needed.
      */
     virtual void deinit() const = 0;

     /** Return the single instance mainwindow of ceb QMainWindow
      */
     virtual QMainWindow *getMainWindow() const = 0;
 };

 Q_DECLARE_INTERFACE(CeBInterface, "ceb.plugin.interface")

 #endif
