#include "lua_qobject.h"

class QColorGroup; // fucking bug in qt3support
#include <QTextBrowser>
#include <QProgressBar>
#include <QTabBar>
#include <QSocketNotifier>
#include <QDockWidget>
#include <QSplashScreen>
#include <QItemSelectionModel>
#include <QActionGroup>
#include <QDoubleSpinBox>
#include <QMainWindow>
#include <QToolBox>
#include <QAbstractEventDispatcher>
#include <QTabWidget>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QGroupBox>
#include <QWidget>
#include <QAction>
#include <QMenuBar>
#include <QTableWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QButtonGroup>
#include <QAbstractSocket>
#include <QMenu>
#include <QTextDocument>
#include <QTreeWidget>
#include <QToolBar>
#include <QIODevice>
#include <QClipboard>
#include <QCheckBox>
#include <QDrag>
#include <QHttp>
#include <QApplication>
#include <QTimer>
#include <QAbstractItemDelegate>
#include <QAbstractTextDocumentLayout>
#include <QLineEdit>
#include <QShortcut>
#include <QToolButton>
#include <QFileDialog>
#include <QStackedLayout>
#include <QCoreApplication>
#include <QAbstractSlider>
#include <QSplitter>
#include <QStatusBar>
#include <QAbstractSpinBox>
#include <QStackedWidget>
#include <QThread>
#include <QTcpServer>
#include <QMovie>
#include <QLCDNumber>
#include <QSignalMapper>
#include <QAbstractItemView>
#include <QProcess>
#include <QFtp>
#include <QProgressDialog>
#include <QTreeView>
#include <QObject>
#include <QSpinBox>
#include <QWorkspace>
#include <QHeaderView>
#include <QDesktopWidget>

class LuaQObject___QAction__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QAction* var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QAction");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QAction__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QWidget__QAbstractItemDelegate__EndEditHint_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QWidget* var1,QAbstractItemDelegate::EndEditHint var2) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QWidget");
			tolua_pushnumber(lua_slot.ls, (lua_Number)var2);
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___QWidget__QAbstractItemDelegate__EndEditHint_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QHttpResponseHeader__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QHttpResponseHeader& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QHttpResponseHeader");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QHttpResponseHeader__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___int_int_int_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(int var1,int var2,int var3) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			tolua_pushnumber(lua_slot.ls, (lua_Number)var2);
			tolua_pushnumber(lua_slot.ls, (lua_Number)var3);
			lua_dbcall(lua_slot.ls, 3, 0);
	};

public:
	LuaQObject___int_int_int_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QImageReader__ImageReaderError_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QImageReader::ImageReaderError var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QImageReader__ImageReaderError_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___qint64_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(qint64 var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___qint64_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QPoint__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QPoint& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QPoint");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QPoint__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___int_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(int var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___int_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___bool_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(bool var1) {

			lua_slot.push_call();
			tolua_pushboolean(lua_slot.ls, (bool)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___bool_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject____ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot() {

			lua_slot.push_call();
			lua_dbcall(lua_slot.ls, 0, 0);
	};

public:
	LuaQObject____(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QTableWidgetItem__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QTableWidgetItem* var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QTableWidgetItem");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QTableWidgetItem__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QTreeWidgetItem__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QTreeWidgetItem* var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QTreeWidgetItem");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QTreeWidgetItem__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QStringList__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QStringList& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QStringList");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QStringList__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QWidget__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QWidget* var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QWidget");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QWidget__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QTextCursor__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QTextCursor& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QTextCursor");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QTextCursor__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QModelIndex__const_QModelIndex__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QModelIndex& var1,const QModelIndex& var2) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QModelIndex");
			tolua_pushusertype(lua_slot.ls, (void*)&var2, "const QModelIndex");
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___const_QModelIndex__const_QModelIndex__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___Qt__DropAction_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(Qt::DropAction var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___Qt__DropAction_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QAbstractButton__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QAbstractButton* var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QAbstractButton");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QAbstractButton__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___int_QHeaderView__ResizeMode_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(int var1,QHeaderView::ResizeMode var2) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			tolua_pushnumber(lua_slot.ls, (lua_Number)var2);
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___int_QHeaderView__ResizeMode_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___int_bool_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(int var1,bool var2) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			tolua_pushboolean(lua_slot.ls, (bool)var2);
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___int_bool_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QTableWidgetItem__QTableWidgetItem__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QTableWidgetItem* var1,QTableWidgetItem* var2) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QTableWidgetItem");
			tolua_pushusertype(lua_slot.ls, (void*)var2, "QTableWidgetItem");
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___QTableWidgetItem__QTableWidgetItem__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___Qt__Orientation_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(Qt::Orientation var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___Qt__Orientation_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QDateTime__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QDateTime& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QDateTime");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QDateTime__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QAbstractSocket__SocketError_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QAbstractSocket::SocketError var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QAbstractSocket__SocketError_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QTreeWidgetItem__QTreeWidgetItem__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QTreeWidgetItem* var1,QTreeWidgetItem* var2) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QTreeWidgetItem");
			tolua_pushusertype(lua_slot.ls, (void*)var2, "QTreeWidgetItem");
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___QTreeWidgetItem__QTreeWidgetItem__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___int_int_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(int var1,int var2) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			tolua_pushnumber(lua_slot.ls, (lua_Number)var2);
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___int_int_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QTreeWidgetItem__int_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QTreeWidgetItem* var1,int var2) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QTreeWidgetItem");
			tolua_pushnumber(lua_slot.ls, (lua_Number)var2);
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___QTreeWidgetItem__int_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QListWidgetItem__QListWidgetItem__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QListWidgetItem* var1,QListWidgetItem* var2) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QListWidgetItem");
			tolua_pushusertype(lua_slot.ls, (void*)var2, "QListWidgetItem");
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___QListWidgetItem__QListWidgetItem__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QListWidgetItem__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QListWidgetItem* var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QListWidgetItem");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QListWidgetItem__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QUrl__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QUrl& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QUrl");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QUrl__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QTextCharFormat__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QTextCharFormat& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QTextCharFormat");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QTextCharFormat__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QItemSelection__const_QItemSelection__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QItemSelection& var1,const QItemSelection& var2) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QItemSelection");
			tolua_pushusertype(lua_slot.ls, (void*)&var2, "const QItemSelection");
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___const_QItemSelection__const_QItemSelection__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QUrlInfo__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QUrlInfo& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QUrlInfo");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QUrlInfo__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QModelIndex__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QModelIndex& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QModelIndex");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QModelIndex__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QDate__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QDate& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QDate");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QDate__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QTime__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QTime& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QTime");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QTime__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___qint64_qint64_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(qint64 var1,qint64 var2) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			tolua_pushnumber(lua_slot.ls, (lua_Number)var2);
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___qint64_qint64_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___double_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(double var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___double_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___Qt__DockWidgetAreas_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(Qt::DockWidgetAreas var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___Qt__DockWidgetAreas_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___Qt__ToolButtonStyle_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(Qt::ToolButtonStyle var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___Qt__ToolButtonStyle_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QString__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QString& var1) {

			lua_slot.push_call();
			tolua_pushqtstring(lua_slot.ls, (const char*)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QString__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QObject__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QObject* var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)var1, "QObject");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QObject__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___Qt__Orientation_int_int_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(Qt::Orientation var1,int var2,int var3) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			tolua_pushnumber(lua_slot.ls, (lua_Number)var2);
			tolua_pushnumber(lua_slot.ls, (lua_Number)var3);
			lua_dbcall(lua_slot.ls, 3, 0);
	};

public:
	LuaQObject___Qt__Orientation_int_int_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QProcess__ProcessError_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QProcess::ProcessError var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QProcess__ProcessError_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QProcess__ProcessState_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QProcess::ProcessState var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QProcess__ProcessState_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QSize__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QSize& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QSize");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QSize__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QRect__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QRect& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QRect");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QRect__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QRectF__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QRectF& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QRectF");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QRectF__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QDockWidget__DockWidgetFeatures_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QDockWidget::DockWidgetFeatures var1) {

			lua_slot.push_call();
			tolua_pushqtflags(lua_slot.ls, (int)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QDockWidget__DockWidgetFeatures_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___int_const_QString__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(int var1,const QString& var2) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			tolua_pushqtstring(lua_slot.ls, (const char*)var2);
			lua_dbcall(lua_slot.ls, 2, 0);
	};

public:
	LuaQObject___int_const_QString__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___Qt__ToolBarAreas_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(Qt::ToolBarAreas var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___Qt__ToolBarAreas_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QAbstractSocket__SocketState_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QAbstractSocket::SocketState var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QAbstractSocket__SocketState_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___const_QSizeF__ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(const QSizeF& var1) {

			lua_slot.push_call();
			tolua_pushusertype(lua_slot.ls, (void*)&var1, "const QSizeF");
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___const_QSizeF__(QObject* p_parent) : LuaQObject(p_parent) {};
};

class LuaQObject___QMovie__MovieState_ : public LuaQObject {

	Q_OBJECT;

public slots:
	void slot(QMovie::MovieState var1) {

			lua_slot.push_call();
			tolua_pushnumber(lua_slot.ls, (lua_Number)var1);
			lua_dbcall(lua_slot.ls, 1, 0);
	};

public:
	LuaQObject___QMovie__MovieState_(QObject* p_parent) : LuaQObject(p_parent) {};
};

class QtSignalHandler__lua_qt : public QtSignalHandler {

public:
	void init_signal_list();
	LuaQObject* get_q_object(QString p_parameters, QObject* p_parent);
	QtSignalHandler__lua_qt() {
		init_signal_list();
	};
};

void QtSignalHandler__lua_qt::init_signal_list() {
	signal_list["(QAction*)"] = 1;
	signal_list["(QWidget*,QAbstractItemDelegate::EndEditHint)"] = 2;
	signal_list["(const QHttpResponseHeader&)"] = 3;
	signal_list["(int,int,int)"] = 4;
	signal_list["(QImageReader::ImageReaderError)"] = 5;
	signal_list["(qint64)"] = 6;
	signal_list["(const QPoint&)"] = 7;
	signal_list["(int)"] = 8;
	signal_list["(bool)"] = 9;
	signal_list["()"] = 10;
	signal_list["(QTableWidgetItem*)"] = 11;
	signal_list["(QTreeWidgetItem*)"] = 12;
	signal_list["(const QStringList&)"] = 13;
	signal_list["(QWidget*)"] = 14;
	signal_list["(const QTextCursor&)"] = 15;
	signal_list["(const QModelIndex&,const QModelIndex&)"] = 16;
	signal_list["(Qt::DropAction)"] = 17;
	signal_list["(QAbstractButton*)"] = 18;
	signal_list["(int,QHeaderView::ResizeMode)"] = 19;
	signal_list["(int,bool)"] = 20;
	signal_list["(QTableWidgetItem*,QTableWidgetItem*)"] = 21;
	signal_list["(Qt::Orientation)"] = 22;
	signal_list["(const QDateTime&)"] = 23;
	signal_list["(QAbstractSocket::SocketError)"] = 24;
	signal_list["(QTreeWidgetItem*,QTreeWidgetItem*)"] = 25;
	signal_list["(int,int)"] = 26;
	signal_list["(QTreeWidgetItem*,int)"] = 27;
	signal_list["(QListWidgetItem*,QListWidgetItem*)"] = 28;
	signal_list["(QListWidgetItem*)"] = 29;
	signal_list["(const QUrl&)"] = 30;
	signal_list["(const QTextCharFormat&)"] = 31;
	signal_list["(const QItemSelection&,const QItemSelection&)"] = 32;
	signal_list["(const QUrlInfo&)"] = 33;
	signal_list["(const QModelIndex&)"] = 34;
	signal_list["(const QDate&)"] = 35;
	signal_list["(const QTime&)"] = 36;
	signal_list["(qint64,qint64)"] = 37;
	signal_list["(double)"] = 38;
	signal_list["(Qt::DockWidgetAreas)"] = 39;
	signal_list["(Qt::ToolButtonStyle)"] = 40;
	signal_list["(const QString&)"] = 41;
	signal_list["(QObject*)"] = 42;
	signal_list["(Qt::Orientation,int,int)"] = 43;
	signal_list["(QProcess::ProcessError)"] = 44;
	signal_list["(QProcess::ProcessState)"] = 45;
	signal_list["(const QSize&)"] = 46;
	signal_list["(const QRect&)"] = 47;
	signal_list["(const QRectF&)"] = 48;
	signal_list["(QDockWidget::DockWidgetFeatures)"] = 49;
	signal_list["(int,const QString&)"] = 50;
	signal_list["(Qt::ToolBarAreas)"] = 51;
	signal_list["(QAbstractSocket::SocketState)"] = 52;
	signal_list["(const QSizeF&)"] = 53;
	signal_list["(QMovie::MovieState)"] = 54;
};

LuaQObject* QtSignalHandler__lua_qt::get_q_object(QString p_parameters, QObject* p_parent) {

	if (signal_list.find(p_parameters) == signal_list.end()) return NULL;

	switch(signal_list[p_parameters]) {

		case 1: return new LuaQObject___QAction__(p_parent);
		case 2: return new LuaQObject___QWidget__QAbstractItemDelegate__EndEditHint_(p_parent);
		case 3: return new LuaQObject___const_QHttpResponseHeader__(p_parent);
		case 4: return new LuaQObject___int_int_int_(p_parent);
		case 5: return new LuaQObject___QImageReader__ImageReaderError_(p_parent);
		case 6: return new LuaQObject___qint64_(p_parent);
		case 7: return new LuaQObject___const_QPoint__(p_parent);
		case 8: return new LuaQObject___int_(p_parent);
		case 9: return new LuaQObject___bool_(p_parent);
		case 10: return new LuaQObject____(p_parent);
		case 11: return new LuaQObject___QTableWidgetItem__(p_parent);
		case 12: return new LuaQObject___QTreeWidgetItem__(p_parent);
		case 13: return new LuaQObject___const_QStringList__(p_parent);
		case 14: return new LuaQObject___QWidget__(p_parent);
		case 15: return new LuaQObject___const_QTextCursor__(p_parent);
		case 16: return new LuaQObject___const_QModelIndex__const_QModelIndex__(p_parent);
		case 17: return new LuaQObject___Qt__DropAction_(p_parent);
		case 18: return new LuaQObject___QAbstractButton__(p_parent);
		case 19: return new LuaQObject___int_QHeaderView__ResizeMode_(p_parent);
		case 20: return new LuaQObject___int_bool_(p_parent);
		case 21: return new LuaQObject___QTableWidgetItem__QTableWidgetItem__(p_parent);
		case 22: return new LuaQObject___Qt__Orientation_(p_parent);
		case 23: return new LuaQObject___const_QDateTime__(p_parent);
		case 24: return new LuaQObject___QAbstractSocket__SocketError_(p_parent);
		case 25: return new LuaQObject___QTreeWidgetItem__QTreeWidgetItem__(p_parent);
		case 26: return new LuaQObject___int_int_(p_parent);
		case 27: return new LuaQObject___QTreeWidgetItem__int_(p_parent);
		case 28: return new LuaQObject___QListWidgetItem__QListWidgetItem__(p_parent);
		case 29: return new LuaQObject___QListWidgetItem__(p_parent);
		case 30: return new LuaQObject___const_QUrl__(p_parent);
		case 31: return new LuaQObject___const_QTextCharFormat__(p_parent);
		case 32: return new LuaQObject___const_QItemSelection__const_QItemSelection__(p_parent);
		case 33: return new LuaQObject___const_QUrlInfo__(p_parent);
		case 34: return new LuaQObject___const_QModelIndex__(p_parent);
		case 35: return new LuaQObject___const_QDate__(p_parent);
		case 36: return new LuaQObject___const_QTime__(p_parent);
		case 37: return new LuaQObject___qint64_qint64_(p_parent);
		case 38: return new LuaQObject___double_(p_parent);
		case 39: return new LuaQObject___Qt__DockWidgetAreas_(p_parent);
		case 40: return new LuaQObject___Qt__ToolButtonStyle_(p_parent);
		case 41: return new LuaQObject___const_QString__(p_parent);
		case 42: return new LuaQObject___QObject__(p_parent);
		case 43: return new LuaQObject___Qt__Orientation_int_int_(p_parent);
		case 44: return new LuaQObject___QProcess__ProcessError_(p_parent);
		case 45: return new LuaQObject___QProcess__ProcessState_(p_parent);
		case 46: return new LuaQObject___const_QSize__(p_parent);
		case 47: return new LuaQObject___const_QRect__(p_parent);
		case 48: return new LuaQObject___const_QRectF__(p_parent);
		case 49: return new LuaQObject___QDockWidget__DockWidgetFeatures_(p_parent);
		case 50: return new LuaQObject___int_const_QString__(p_parent);
		case 51: return new LuaQObject___Qt__ToolBarAreas_(p_parent);
		case 52: return new LuaQObject___QAbstractSocket__SocketState_(p_parent);
		case 53: return new LuaQObject___const_QSizeF__(p_parent);
		case 54: return new LuaQObject___QMovie__MovieState_(p_parent);

		default: return NULL;
	};
	return NULL;
};

QtSignalHandler* new_QtSignalHandler__lua_qt() {
	return new QtSignalHandler__lua_qt;
};

#include "qt_signal_handlers_lua_qt.moc"
