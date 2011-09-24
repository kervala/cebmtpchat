 class PluginInterface
 {
 public:
     virtual ~PluginInterface() {}

     virtual void init() const = 0;
     virtual void deinit() const = 0;

     virtual QMainWindow *getMainWindow() const = 0;
 };

 Q_DECLARE_INTERFACE(PluginInterface, "ceb.plugin.interface")
