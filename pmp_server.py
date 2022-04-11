################################################################################
# PMP Server
# Copyright (C) 2006 David Voswinkel
# 
# Homepage: http://optixx.org
# E-mail:   david@optixx.org
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
################################################################################

#################################################################################
#$LastChangedDate: 2006-02-25 17:35:28 +0100 (Sa, 25 Feb 2006) $
#$LastChangedRevision: 12 $
#$LastChangedBy: david $
#$HeadURL: http://127.0.0.1/svn/repos/pmp_mod/trunk/pmp_server.py $
#$Id: pmp_server.py 12 2006-02-25 16:35:28Z david $
#################################################################################

# curl "http://localhost:3333/open?filename=movie.avi&flags=O_RDONLY" 
# curl "http://localhost:3333/read?fd=1&size=100" 
# curl "http://localhost:3333/lseek?fd=1&offset=1000&whence=1"
# curl "http://localhost:3333/close?fd=1"




import re
import os
import sys
import string
import linecache
import traceback
import BaseHTTPServer
import threading
import urllib
import urlparse
import types
import urllib
import time 
import platform

   
if platform.system() in ['Windows','Microsoft']:
    ROOT="C:\Movies"
else:
    ROOT="/movies"

try:
    from qt import *
    try:
	from qt import QThread
    except:
	print "Thread support not enabled"
	sys.exit(1)
    GUI = True
except:
    GUI = False
 
try:
    from kdecore import KApplication, KIcon, KIconLoader, KCmdLineArgs, KAboutData
    from kdeui import KMainWindow, KSystemTray
    KDE = True
except:
    KDE = False

image_exit_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x16\x00\x00\x00\x16" \
    "\x08\x06\x00\x00\x00\xc4\xb4\x6c\x3b\x00\x00\x00" \
    "\xa0\x49\x44\x41\x54\x38\x8d\xd5\x95\x4d\x0a\x80" \
    "\x20\x10\x85\x9f\xd1\x46\x68\xe1\x8d\xe6\x62\xd2" \
    "\x22\xbc\x98\x37\x6a\x21\xb4\xac\x45\x19\x92\xc6" \
    "\x64\x69\xe0\xb7\xf1\x87\xf1\xf1\x1c\x47\x05\x2a" \
    "\x21\x8e\x76\x2d\xad\xdb\xfb\x9e\x99\xf6\x56\x8f" \
    "\x80\xb5\x36\x4b\x85\x88\xce\x35\x44\x04\x00\xe8" \
    "\x0a\x39\x8c\xe8\xf9\x90\x34\xd2\x29\x2c\xc3\x7c" \
    "\x8e\xbd\x53\x0f\xeb\x58\x3a\x05\xe9\x54\x34\x1f" \
    "\x8a\x02\x7b\x2a\x7d\x3a\x1f\x09\xbf\x85\x4d\xc5" \
    "\xd5\xd9\x53\xaa\x39\x6e\x4f\x38\xca\xb1\x99\xe2" \
    "\xd2\xe1\x08\xab\xe1\x56\xf8\x2e\x30\x97\x7f\xcb" \
    "\x4d\x8f\xf9\x42\xd7\x5d\xbe\xbe\xd2\xe1\x43\x95" \
    "\x3a\x93\xf6\xca\xad\x3d\x61\x11\xf4\x4b\x7d\x4f" \
    "\x82\x0f\xf9\xc0\x06\x9b\xb5\x1e\xcd\xed\x31\x8c" \
    "\x5c\x00\x00\x00\x00\x49\x45\x4e\x44\xae\x42\x60" \
    "\x82"



class App(object):
    
    def __new__(cls, *args, **kwds):
        gui  = args[0]
	root = args[1]
	if gui:
	    obj = GuiApp.__new__(GuiApp)
	else:
	    obj = CliApp.__new__(CliApp)
	obj.__init__(root)
        return obj 
  
class CliApp(object):

    def __init__(self,root):
	self.root = root 
    
    def run(self):
        banner()
	try:
	    httpd = HTTPServer(('',3333),FileIOServer)
	    httpd.register(self)
	    httpd.setRoot(self.root)
	    while True: 
		httpd.handle_request()
	except Exception, argument:
	    et, ev, tb = sys.exc_info()
	    sys.stderr.write('#' * 60 + '\n')
	    sys.stderr.write('Exception:%s %s\n' % (et,ev))
	    while tb:
		co = tb.tb_frame.f_code
		filename = co.co_filename
		linenum =  traceback.tb_lineno(tb),
		line = linecache.getline(filename,linenum[0])
		line = re.compile('[\n\t]+|[ ]{2,1000}').sub(' ',line)
		sys.stderr.write("File: %s Line: %6s Code: '%s' \n" % (filename,linenum[0],line))
		tb = tb.tb_next
	    sys.stderr.write('#' * 60 + '\n')
	return

    def log(self,s,url,action,net,fd,filename,status):
	ts = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())
	sys.stdout.write("[%s] [%s] [%06s] net:%03i fd:%03i '%s' '%s'\n" % (ts,url[:40],action,net,fd,filename,status)) 
    
    def error(self,url,msg):
	ts = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())
	sys.stdout.write("[%s] [%s] ERROR: %s\n" % (ts,url,msg))
    
    def progress(self,filename,percent):
	pass 
    
    def average(self,filename,avr):
	pass

class Worker(QThread):
    
    def __init__(self,o,r):
        QThread.__init__(self)
        self.receiver = o
	self.stopped = 0
	self.root = r
    
    def run(self):
        self.stopped = 0
	try:
	    httpd = HTTPServer(('',3333),FileIOServer)
	    httpd.register(self) 
	    httpd.setRoot(self.root)
	    while not self.stopped: 
		httpd.handle_request()
	except Exception, argument:
	    et, ev, tb = sys.exc_info()
	    sys.stderr.write('#' * 60 + '\n')
	    sys.stderr.write('Exception:%s %s\n' % (et,ev))
	    while tb:
		co = tb.tb_frame.f_code
		filename = co.co_filename
		linenum =  traceback.tb_lineno(tb),
		line = linecache.getline(filename,linenum[0])
		line = re.compile('[\n\t]+|[ ]{2,1000}').sub(' ',line)
		sys.stderr.write("File: %s Line: %6s Code: '%s' \n" % (filename,linenum[0],line))
		tb = tb.tb_next
	    sys.stderr.write('#' * 60 + '\n')
	sys.stderr.write("Thread going down\n")
	return

    def stop(self):
	self.stopped = 1

    def log(self,url,action,net,fd,filename,status):
	ts = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())
	sys.stdout.write("[%s] [%-40s] [%-6s] net:%03i fd:%03i '%s' %s\n" % (ts,url[:40],action,net,fd,filename,status)) 
	s = "%s|%s|%s|%s" %  (url,action,filename,status)
	event = QCustomEvent(12345)
	event.setData(QString(s))
	QThread.postEvent(self.receiver,event)
	
   
    def error(self,url,msg):
	ts = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())
	sys.stdout.write("[%s] [%-40s] ERROR: %s\n" % (ts,url[:40],msg))
	s = "%s|%s" % (url,msg)
	event = QCustomEvent(12346)
	event.setData(QString(s))
	QThread.postEvent(self.receiver,event)
	
    def progress(self,filename,percent):
	s = "%s|%2.3f" % (filename,percent)
	event = QCustomEvent(12347)
	event.setData(QString(s))
	QThread.postEvent(self.receiver,event)
	
    def average(self,filename,avr):
	s = "%s|%2.3f" % (filename,avr)
	event = QCustomEvent(12348)
	event.setData(QString(s))
	QThread.postEvent(self.receiver,event)
	
class PMPFrontend(QMainWindow):
    
    def __init__(self,parent = None,name = None,fl = 0,root = ''):
        QMainWindow.__init__(self,parent,name,fl)
	self.root = root	
	self.worker = Worker(self,self.root)
        self.image_exit = QPixmap()
        self.image_exit.loadFromData(image_exit_data,"PNG")
	self.setupLayout()
	self.lineedit_directory.setText(self.root)
        
	self.timer = QTimer(self, "cleanup")
	self.connect(self.timer, SIGNAL("timeout()"), self.cleanup)
	self.startServer(False)

    
    def setupLayout(self):
      
	self.setMinimumSize(QSize(400,300))
        self.setMaximumSize(QSize(600,400))

        
	self.setCentralWidget(QWidget(self,"qt_central_widget"))
        PMPFrontendLayout = QGridLayout(self.centralWidget(),1,1,11,6,"PMPFrontendLayout")

        self.tab = QTabWidget(self.centralWidget(),"tab")

        self.tab_status = QWidget(self.tab,"tab_status")

        self.frame = QFrame(self.tab_status,"frame")
        self.frame.setGeometry(QRect(10,10,342,157))
        self.frame.setFrameShape(QFrame.StyledPanel)
        self.frame.setFrameShadow(QFrame.Raised)

        self.text_action = QLabel(self.frame,"text_action")
        self.text_action.setGeometry(QRect(11,40,320,21))
        self.text_action.setFrameShape(QLabel.TabWidgetPanel)

        self.text_status = QLabel(self.frame,"text_status")
        self.text_status.setGeometry(QRect(11,94,320,21))
        self.text_status.setFrameShape(QLabel.TabWidgetPanel)

        self.progress_bar = QProgressBar(self.frame,"progress_bar")
        self.progress_bar.setGeometry(QRect(11,121,320,25))

        self.text_filename = QLabel(self.frame,"text_filename")
        self.text_filename.setGeometry(QRect(11,67,320,21))
        self.text_filename.setFrameShape(QLabel.TabWidgetPanel)

        self.lCDNumber = QLCDNumber(self.frame,"lCDNumber")
        self.lCDNumber.setGeometry(QRect(11,11,64,23))
        self.lCDNumber.setFrameShape(QLCDNumber.TabWidgetPanel)
        self.lCDNumber.setNumDigits(6)
        self.tab.insertTab(self.tab_status,QString.fromLatin1(""))

        self.tab_config = QWidget(self.tab,"tab_config")

        self.button_select = QPushButton(self.tab_config,"button_select")
        self.button_select.setGeometry(QRect(292,11,71,26))

        self.lineedit_directory = QLineEdit(self.tab_config,"lineedit_directory")
        self.lineedit_directory.setGeometry(QRect(11,12,275,24))

        self.button_restart = QPushButton(self.tab_config,"button_restart")
        self.button_restart.setGeometry(QRect(239,163,124,26))

        self.button_start = QPushButton(self.tab_config,"button_start")
        self.button_start.setGeometry(QRect(124,163,109,26))

        self.button_stop = QPushButton(self.tab_config,"button_stop")
        self.button_stop.setGeometry(QRect(11,163,107,26))
        self.tab.insertTab(self.tab_config,QString.fromLatin1(""))

        PMPFrontendLayout.addWidget(self.tab,0,0)

        self.fileSaveAction = QAction(self,"fileSaveAction")
        self.fileSaveAction.setIconSet(QIconSet(self.image_exit))
        self.fileExitAction = QAction(self,"fileExitAction")
        self.helpAboutAction = QAction(self,"helpAboutAction")




        self.MenuBar = QMenuBar(self,"MenuBar")


        self.fileMenu = QPopupMenu(self)
        self.fileSaveAction.addTo(self.fileMenu)
        self.fileMenu.insertSeparator()
        self.fileMenu.insertSeparator()
        self.fileExitAction.addTo(self.fileMenu)
        self.MenuBar.insertItem(QString(""),self.fileMenu,1)

        self.helpMenu = QPopupMenu(self)
        self.helpMenu.insertSeparator()
        self.helpAboutAction.addTo(self.helpMenu)
        self.MenuBar.insertItem(QString(""),self.helpMenu,2)


        self.languageChange()

        self.resize(QSize(400,300).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.connect(self.fileSaveAction,SIGNAL("activated()"),self.fileSave)
        self.connect(self.fileExitAction,SIGNAL("activated()"),self.fileExit)
        self.connect(self.helpAboutAction,SIGNAL("activated()"),self.helpAbout)
        self.connect(self.button_select,SIGNAL("clicked()"),self.selectDir)
        self.connect(self.button_restart,SIGNAL("clicked()"),self.restartServer)
        self.connect(self.button_start,SIGNAL("clicked()"),self.startServer)
        self.connect(self.button_stop,SIGNAL("clicked()"),self.stopServer)


	    
    
    def cleanup(self):	
	self.text_action.setText("")
        self.text_status.setText("")
        self.text_filename.setText("")
	self.progress_bar.setProgress(0)
	self.lCDNumber.display(0)
	self.timer.stop()
	
    def languageChange(self):
	self.setCaption(self.__tr("PMP Server"))
        self.text_action.setText(QString.null)
        self.text_status.setText(QString.null)
        self.text_filename.setText(QString.null)
        self.tab.changeTab(self.tab_status,self.__tr("Status"))
        self.button_select.setText(self.__tr("Select"))
        self.button_restart.setText(self.__tr("Restart Server"))
        self.button_start.setText(self.__tr("Start Server"))
        self.button_stop.setText(self.__tr("Stop Server"))
        self.tab.changeTab(self.tab_config,self.__tr("Config"))
        self.fileSaveAction.setText(self.__tr("Save"))
        self.fileSaveAction.setMenuText(self.__tr("&Save"))
        self.fileSaveAction.setAccel(self.__tr("Ctrl+S"))
        self.fileExitAction.setText(self.__tr("Exit"))
        self.fileExitAction.setMenuText(self.__tr("E&xit"))
        self.fileExitAction.setAccel(QString.null)
        self.helpAboutAction.setText(self.__tr("About"))
        self.helpAboutAction.setMenuText(self.__tr("&About"))
        self.helpAboutAction.setAccel(QString.null)
        if self.MenuBar.findItem(1):
            self.MenuBar.findItem(1).setText(self.__tr("&File"))
        if self.MenuBar.findItem(2):
            self.MenuBar.findItem(2).setText(self.__tr("&Help"))


      
    def fileSave(self):
        print "PMPFrontend.fileSave(): Not implemented yet"

    def fileExit(self):
	self.stopServer()
	sys.exit()
	
    def helpAbout(self):
	QMessageBox.about( self, "PMP Server",
		"This is the PMP Server for network enabled\n"
		"pmp_mod from www.optixx.org\n")
	  
        print "PMPFrontend.helpAbout(): Not implemented yet"

    def selectDir(self):    
	fn = QFileDialog.getExistingDirectory( QString.null,self )
	if not fn.isEmpty():
	    self.lineedit_directory.setText(fn)
	     
    def restartServer(self):
	self.stopServer()
	self.worker.root =  str(self.lineedit_directory.text().latin1())
	self.startServer()

    def __tr(self,s,c = None):
        return qApp.translate("PMPFrontend",s,c)   

        QWidget.__init__(self)
        self.worker = Worker(self)

            
    def stopServer(self):
	if not  self.worker.running():
	    return 
	sys.stderr.write("Shutting down\n") 
        stopWorker = self.worker.running()
        self.worker.stop()
	#HACK: Server has to recheck while condition
	urllib.urlopen("http://localhost:3333/dummy")
	self.worker.wait()
       

    def startServer(self,display=True):
        if self.worker.running():
            QMessageBox.information(self,"Sorry",
                "The threads have not completed yet, and must finish before "
                "they can be started again.")

            return
        self.worker.start()
	if display:
	    QMessageBox.information(self,"Information",
                "Server started, using '%s'" %  self.lineedit_directory.text())


    
    def customEvent(self,event):
        
	if self.timer.isActive():
	    self.timer.stop()
	    
	self.timer.start(100 * 30)
	
	if event.type() == 12345:
            s = event.data()
	    msg = s.latin1()
	    if msg.find("|"):
		val = msg.split("|")
		self.text_action.setText(val[1])
        	self.text_filename.setText(os.path.basename(val[2]))
		self.text_status.setText(val[3])
	    del s
	    
	elif event.type() == 12346: 
	    s = event.data()
	    msg = s.latin1()
	    if msg.find("|"):
		val = msg.split("|")
		self.text_action.setText("")
		self.text_filename.setText("")
		self.text_status.setText(val[1])
		QMessageBox.warning(self,"Error",val[1])
	    del s
	    
	elif event.type() == 12347: 
	    s = event.data()
	    msg = s.latin1()
	    if msg.find("|"):
		val = msg.split("|")
		filename = os.path.basename(val[0])
		percent = float(val[1])
		self.progress_bar.setProgress(percent)
	    del s
	    
	elif event.type() == 12348: 
	    s = event.data()
	    msg = s.latin1()
	    if msg.find("|"):
		val = msg.split("|")
		filename = os.path.basename(val[0])
		avr = int(float(val[1]))
		self.lCDNumber.display(avr)
	    del s
	
	else:
            print "Unknown custom event type:", event.type()


class GuiApp(object):

    def __init__(self,root):
	self.root = root 

    def run(self):
	if KDE:      
	    description = "Server for PMP_MOD"
	    version = "1.0"
	    aboutData   = KAboutData ("pmp_server", "PMP Server",\
		version, description, KAboutData.License_GPL,\
		   "(C) 2006 optixx.org")
	    KCmdLineArgs.init (sys.argv, aboutData)
	    app = KApplication()
	    p = PMPFrontend(None,"PMP Server",0,self.root)
	    icons = KIconLoader ()
	    systray = KSystemTray (p)
	    systray.setPixmap (icons.loadIcon("server",KIcon.Toolbar))
	    systray.show()
	    systray.connect(systray,SIGNAL("quitSelected()"),p.fileExit)
	    p.show()

	
	else:
	    app = QApplication(sys.argv)
	    p = PMPFrontend(None,"PMP Server",0,self.root)
	    app.setMainWidget(p)
	    p.show()
	sys.exit(app.exec_loop())
	

def banner():
    sys.stdout.write("#"*80 +"\n")
    sys.stdout.write("# PMP Server by www.optixx.org\n")
    sys.stdout.write("#"*80 +"\n")
    sys.stdout.write("ROOT='%s'\n" % ROOT)
     




class HTTPServer(BaseHTTPServer.HTTPServer):

    def __init__(self,addr,handler):
        BaseHTTPServer.HTTPServer.__init__(self,addr,handler)
        self.RequestHandlerClass.subjects = {}
	self.fds = {}
    
    def register(self,p):
	self.RequestHandlerClass.p = p
    
    def setRoot(self,r):
	sys.stdout.write("ROOT='%s'\n" % r)
	self.RequestHandlerClass.root =r

class RingBuffer(object):

    def __init__(self,size):
        self.size = size
        self.data = []

    def add(self,v):
        if len(self.data) < self.size:
            self.data.append(v)
        else:
            del self.data[0]
            self.data.append(v)

    def average(self):
        t = float()
        for i in self.data:
            t += i
        if t:
            return float(t/len(self.data))
        else:
            return 0

class FileIOObject:

    def __init__(self,fd,name,size):
	self.name = name
	self.mutex = threading.Semaphore(1)
	self.fd = fd
	self.size = size
	self.offset = 0
	self.last_time	 = 0 
	self.last_offset = 0
	self.band = RingBuffer(10)
	

class FileIOServer(BaseHTTPServer.BaseHTTPRequestHandler):

    __fds      = {}
    __fd_max   = 0
    __mutex    = threading.Semaphore(1) 
    server_version = "PMPServer"
  
    def log_message(self, format, *args):
        sys.stdout.write("[%s] %s\n" % (self.address_string(),format%args))

    def do_POST(self):
        self.do_get()
        
    def do_GET(self):
	uri = urlparse.urlparse(self.path)
	action 	= uri[2].replace("/","")
	args    = uri[4].split("&")
	if not isinstance(args,types.ListType):
	    self.send(str(0))
	    return 
	if action not in ['open','close','lseek','read','listdir','stat','fstat']:
	   self.send(str(0))
	   return 
	kwds = {}
	for arg in args:
	    if arg.find("=")<0:
		continue
	    tmp = arg.split("=")
	    kwds[tmp[0]] = urllib.unquote(tmp[1])
	args = []
	func = getattr(self, 'FileIO' + action)
	data = func(*args,**kwds)
	self.send(data)
	
    def send(self,data):
	self.wfile.write("HTTP/1.0 200 OK\r\n")
	self.send_header("Content-type","video/mpeg")
	self.end_headers()
	self.wfile.write(data)
	self.wfile.flush()
	
    def FileIOopen(self,*args,**kwds):
	try:
	    filename = os.path.join(self.root,urllib.unquote_plus(kwds['filename']))
	    if not kwds.has_key('flags'):
		flags = "O_RDONLY"
	    else:
		flags = kwds['flags']
	    if not hasattr(os,flags):
		flags = "O_RDONLY"
	    flags = getattr(os,flags)
	    if not os.path.isfile(filename):
		self.p.error(self.path,"no file '%s'" % filename)
		return "-1"
	    FileIOServer.__mutex.acquire()
	    FileIOServer.__fd_max =  FileIOServer.__fd_max +1
	    net_fd = FileIOServer.__fd_max
	    if platform.system() in  ['Windows','Microsoft']: 
		flags |= os.O_BINARY  
	    fileno = os.open(filename,flags)
	    stats = os.fstat(fileno)
	    size = stats[6]
	    fo = FileIOObject(fileno,filename,size)
	    FileIOServer.__fds[net_fd] = fo
	    self.p.log(self.path,"Open",net_fd,fileno,filename,"OK") 
	except:
	    et, ev, tb = sys.exc_info()
	    self.p.error(self.path,"%s %s" % (et,ev))
	    FileIOServer.__mutex.release() 
	    return "-1"
	FileIOServer.__mutex.release()
	return net_fd
    
    def FileIOstat(self,*args,**kwds):
	try:
	    filename = os.path.join(self.root,urllib.unquote_plus(kwds['filename']))
	    if os.path.isfile(filename):
		ret = os.stat(filename)
		ret_buf = str()
		for val in ret:
		    ret_buf += "%i " % val
	    else:
		return "-1"
	    self.p.log(self.path,"Stat",0,0,filename,ret_buf)
	except:
	    et, ev, tb = sys.exc_info()
	    self.p.error(self.path,"%s %s" % (et,ev))
	    return "-1"
	return ret_buf
      
    def FileIOread(self,*args,**kwds):
	try:
	    fo	    = None  
	    net_fd  = int(kwds['fd'])
	    size    = int(kwds['size'])
	    FileIOServer.__mutex.acquire()
	    if not FileIOServer.__fds.has_key(net_fd):
		FileIOServer.__mutex.release()
		return "-1"
	    
	    fo = FileIOServer.__fds[net_fd]
	    fo.mutex.acquire()
	    FileIOServer.__mutex.release()
	    recv_data=os.read(fo.fd,size)
	    recv_len = len(recv_data)
	    time_diff = time.time() - fo.last_time
	    offset_diff = fo.offset + recv_len  - fo.last_offset
	    fo.band.add(offset_diff / 1024 / time_diff)
	    fo.last_offset = fo.offset
	    fo.last_time   = time.time()
	    fo.offset += recv_len
	    FileIOServer.__fds[net_fd] = fo
	    self.p.log(self.path,"Read",net_fd,fo.fd,fo.name,"'%i' of '%i' bytes" % (recv_len,size))
	    self.p.progress(fo.name,(100.0 / fo.size) * fo.offset)
	    self.p.average(fo.name,fo.band.average())
	    fo.mutex.release()    
	except:
	    FileIOServer.__mutex.release() 
	    if fo:
		fo.mutex.release()
	    et, ev, tb = sys.exc_info()
	    self.p.error(self.path,"%s %s" % (et,ev))
	    return "-1"
	
	return recv_data
   
    def FileIOfstat(self,*args,**kwds):
	try:
	    fo	    = None  
	    net_fd  = int(kwds['fd'])
	    FileIOServer.__mutex.acquire()
	    if not FileIOServer.__fds.has_key(net_fd):
		FileIOServer.__mutex.release()
		return "-1"
	    
	    fo = FileIOServer.__fds[net_fd]
	    fo.mutex.acquire()
	    FileIOServer.__mutex.release()
	    ret = os.fstat(fo.fd)
	    ret_buf = str()
	    for val in ret:
		ret_buf += "%i " % val
	    self.p.log(self.path,"Fstat",net_fd,fo.fd,fo.name,ret_buf)
	    fo.mutex.release()    
	except:
	    FileIOServer.__mutex.release() 
	    if fo:
		fo.mutex.release()
	    et, ev, tb = sys.exc_info()
	    self.p.error(self.path,"%s %s" % (et,ev))
	    return "-1"
	
	return ret_buf
    
   
    def FileIOlseek(self,*args,**kwds):
	try:
	    fo	    = None  
	    net_fd  = int(kwds['fd'])
	    offset  = int(kwds['offset'])
	    whence  = int(kwds['whence'])
	    FileIOServer.__mutex.acquire()
	    if not FileIOServer.__fds.has_key(net_fd):
		FileIOServer.__mutex.release()
		return "-1"
	    
	    fo = FileIOServer.__fds[net_fd]
	    fo.mutex.acquire()
	    FileIOServer.__mutex.release()
	    ret  = os.lseek(fo.fd,offset,whence)
	    fo.offset = ret
	    fo.last_offset = ret
	    FileIOServer.__fds[net_fd] = fo
	    fo.mutex.release()    
	    self.p.log(self.path,"Seek",net_fd,fo.fd,fo.name,"offset '%i'" % ret)
	    self.p.progress(fo.name,(100.0 / fo.size) * fo.offset)
	except:
	    FileIOServer.__mutex.release() 
	    if fo:
		fo.mutex.release()
	    et, ev, tb = sys.exc_info()
	    self.p.error(self.path,"%s %s" % (et,ev))
	    return "-1"
	return str(ret)
    
    def FileIOclose(self,*args,**kwds):
	try:
	    fo	    = None  
	    net_fd  = int(kwds['fd'])
	    FileIOServer.__mutex.acquire()
	    if not FileIOServer.__fds.has_key(net_fd):
		FileIOServer.__mutex.release()
		return "-1"
	    
	    fo = FileIOServer.__fds[net_fd]
	    ret  = os.close(fo.fd)
	    self.p.log(self.path,"Close",net_fd,fo.fd,fo.name,"OK")
	    self.p.progress("",0)
	    fo.mutex.release()   
	    del  FileIOServer.__fds[net_fd]
	    FileIOServer.__mutex.release()
	except:
	    FileIOServer.__mutex.release() 
	    if fo:
		fo.mutex.release()
	    et, ev, tb = sys.exc_info()
	    self.p.error(self.path,"%s %s" % (et,ev))
	    return "-1"
	return str(0)
    
    def FileIOlistdir(self,*args,**kwds):
	try:
	    path = os.path.join(self.root,kwds['path'])
	    if kwds.has_key("size"):
		size = int(kwds['size'])
	    else:
		size = 4 * 1024
	    cnt = 0
	    dlist = str()
	    d = os.listdir(path)
	    for dent in d:
		if dent.startswith("."):
		    continue
		ext = dent.split(".")
		if not len(ext):
		    continue
		if ext.pop().lower() not in ['pmp','avi','mpg','mpeg','divx','xvid']:
		    continue
		if os.path.isfile(os.path.join(path,dent)):
		    next = "net:/%s\n" % (dent)
		    if len(dlist) + len(next) > size:
			break 
		    dlist += next 
		    cnt +=1
	    self.p.log(self.path,"Dir",0,0,path,"'%i' files" % cnt)
	except:
	    et, ev, tb = sys.exc_info()
	    self.p.error(self.path,"%s %s" % (et,ev))
	    return "-1"
	return dlist
      
 

if __name__ == '__main__':
    if len(sys.argv)>1 and sys.argv[1] =='nogui':
	GUI = False
    app = App(GUI,ROOT)
    app.run()
    

    
        


