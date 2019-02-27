/////////////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs : WPF Client that uses Shim to call native C++ code  //
// ver 1.0                                                                 //
//                                                                         //
// Application    : CSE-687 C++/CLI Shim demostration                      //
// Platform       : Visual Studio 2017 Community Edition                   //
//                  Windows 10 Professional 64-bit, Acer Aspire R5-571TG   //
// Author         : Ankita Gadage, EECS Department, Syracuse University     //
/////////////////////////////////////////////////////////////////////////////
/*
 *  Package Decription:
 * =====================
 *  This package defines the GUI using MainWindow.xaml file which is compiled
 *  to a working GUI saving you the effort of programatically creating GUIs.
 *  This package depends on Translater project, if Translater project is NOT built then
 *  Translater.dll does not exist and therefore you'll get Intellisense error saying
 *  you have missing reference. 
 *  
 *  Public Interface: N/A
 *  
 *  Required Files:
 * =================
 *  MainWindow.xaml MainWindow.xaml.cs Translater.dll codePopuoWindow.cpp
 *  
 *  Build Process:
 * ================
 *  msbuild WPF.csproj
 *  
 *  Maintainence History:
 * =======================
 *  ver 1.0 - March 23rd, 2018
 *    - first release
 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.IO;
using Microsoft.Win32;
using MsgPassingCommunication;
using System.Collections.ObjectModel;

namespace WpfAppClient
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        //Class for create the ui checkin table
        public class DataObject
        {
            public string A { get; set; }
            public string B { get; set; }
            public string C { get; set; }
            public string D { get; set; }

            public string E { get; set; }

            public string F { get; set; }
            public string G { get; set; }


        }
        //----< Initilaization component >---------------

        public MainWindow()
        {
            InitializeComponent();

        }

        //data declaration
        private Stack<string> pathStack_ = new Stack<string>();
        private Stack<string> checkinPathStack_ = new Stack<string>();
        private Stack<string> checkoutPathStack_ = new Stack<string>();
        private Stack<string> metadataPathStack_ = new Stack<string>();
        public Stack<string> addChildPathStack_ = new Stack<string>();
        ObservableCollection<DataObject> list = new ObservableCollection<DataObject>();
        List<CsMessage> messageList = new List<CsMessage>();

        private bool isAddChildOnTop = false;
        AddChild childBrowseWindow;


        private Translater translater;
        private CsEndPoint endPoint_;
        private CsEndPoint serverEndPoint;
        OpenFileDialog openFileDialog;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();


        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

       
        //----< load estConnection processing into dispatcher dictionary >------

        private void DispatcherConnectToServer()
        {
            Action<CsMessage> connectToServer = (CsMessage rcvMsg) =>
            {
                Action<CsMessage> displayText = (CsMessage lRcvMsg) =>
                {
                    labelConnectionStatus.Content = lRcvMsg.value("content");
                    if(lRcvMsg.value("content").Contains("Connected"))
                    {
                        Thread.Sleep(1000);
                        PathTextBlock.Text = "Storage";
                        //CheckinPathTextBlock.Text = "Storage";
                        CheckOutPathTextBlock.Text = "Storage";
                        MetadataPathTextBlock.Text = "Storage";
                        addChildPathStack_.Push("../Storage");
                        pathStack_.Push("../Storage");
                        checkinPathStack_.Push("../Storage");
                        checkoutPathStack_.Push("../Storage");
                        metadataPathStack_.Push("../Storage");
                        CsMessage msg = new CsMessage();
                        msg.add("to", CsEndPoint.toString(serverEndPoint));
                        msg.add("from", CsEndPoint.toString(endPoint_));
                        msg.add("command", "getDirs");
                        msg.add("path", pathStack_.Peek());
                        translater.postMessage(msg);
                        msg.remove("command");
                        msg.add("command", "getFiles");
                        translater.postMessage(msg);
                        statusBarText.Items.Insert(0, "Connection established with Server ");
                        for (int i = 1; i < 6; i++)
                        {
                            var tab = tabControl.Items[i] as TabItem;
                            tab.IsEnabled = true;
                        }
                    }
                    else
                    {
                        statusBarText.Items.Insert(0, "Connection to Server Failed");
                    }
                };
                Dispatcher.Invoke(displayText, new Object[] { rcvMsg });
            };
            addClientProc("estConnection", connectToServer);
        }

        //----< load checkin processing into dispatcher dictionary >------

        private void DispatcherCheckinResponse()
        {
            Action<CsMessage> connectCheckInResp = (CsMessage rcvMsg) =>
            {
                Action<CsMessage> connectCheckInRespDisplay = (CsMessage lRcvMsg) =>
                {
                    statusBarText.Items.Insert(0, "Response from Server :- Uploading file Successfull "+lRcvMsg.value("fileName"));
                };

                Dispatcher.Invoke(connectCheckInRespDisplay, new Object[] { rcvMsg });
            };
            addClientProc("CheckinResp", connectCheckInResp);
        }
        //----< load checkin processing into dispatcher dictionary >------

        private void idpFileResponse()
        {
            Action<CsMessage> connectIdpResp = (CsMessage rcvMsg) =>
            {
                Action<CsMessage> connectCheckInRespDisplay = (CsMessage lRcvMsg) =>
                {
                    Browse_FileList.Items.Clear();
                    if (lRcvMsg.value("idplist").Contains("NotPresent"))
                    {
                        statusBarText.Items.Insert(0,"No idependent file found");
                    }
                    else
                    {
                        IList<string> names = lRcvMsg.value("idplist").Split('#').ToList<string>();
                        foreach (var p in names)
                        {
                            Browse_FileList.Items.Insert(0,p);

                        }

                    }

                };

                Dispatcher.Invoke(connectCheckInRespDisplay, new Object[] { rcvMsg });
            };
            addClientProc("getIndependentFile", connectIdpResp);
        }

        //----< load getMetadata processing into dispatcher dictionary >------

        private void DispatcherMetadataResponse()
        {
            Action<CsMessage> connectMetadataResp = (CsMessage rcvMsg) =>
            {
                Action<CsMessage> connectMetadataRespDisplay = (CsMessage lRcvMsg) =>
                {
                    statusBarText.Items.Add("Response from Server :- Metadata Recieved");
                    String metadata = "Name:: "+rcvMsg.value("Name") +"\nDateTime::" + rcvMsg.value("DateTime") + "\nDescription::" + rcvMsg.value("Description") + "\nChildren::" + rcvMsg.value("Dependency") ;
                    showFile(rcvMsg.value("Name") , metadata);
                };

                Dispatcher.Invoke(connectMetadataRespDisplay, new Object[] { rcvMsg });
            };
            addClientProc("getMetadata", connectMetadataResp);
        }

        //----< register all dispatcher in dictionary and initialize window >------

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

            DispatcherConnectToServer();
            idpFileResponse();
            DispatcherLoadGetDirs();
            getAuthorResp();
            BrowseFileReply();
            DispatcherCheckinResponse();
            DispatcherLoadGetFile();
            DispatcherLoadGetFiles();
            DispatcherMetadataResponse();
            disconnectButton.IsEnabled = false;
            for (int i = 1; i < 6; i++)
            {
                var tab = tabControl.Items[i] as TabItem;
                tab.IsEnabled = false;
            }

            string[] commandLineArgs = Environment.GetCommandLineArgs();
            
            if (commandLineArgs.Length == 3)
            {
                testStub(commandLineArgs[2]);
            }
            else
            {
                testStub("8082");
                Console.WriteLine("\nStarting new client...");
            }
        }



        //----< function to clear the directory section from all tabs >-------

        private void clearDirs()
        {
            TabItem t1 = tabControl.SelectedItem as TabItem;
            if (t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false && t1.Header.ToString().Equals("View Metadata") == false)
                DirList.Items.Clear();
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("View Metadata") == false)
                CheckOutDirList.Items.Clear();
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false)
                MetadataDirList.Items.Clear();
            if(isAddChildOnTop == true)
            {
                childBrowseWindow.DirList.Items.Clear();
            }
        }

        //----< function to add the directory section from all tabs >-------

        private void addDir(string dir)
        {
            TabItem t1 = tabControl.SelectedItem as TabItem;

            if (t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false && t1.Header.ToString().Equals("View Metadata") == false)
                DirList.Items.Add(dir);
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("View Metadata") == false)
                CheckOutDirList.Items.Add(dir);
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false)
                MetadataDirList.Items.Add(dir);
            if (isAddChildOnTop == true)
            {
                childBrowseWindow.DirList.Items.Add(dir);
            }
        }

        //----< function to load the parent repo directory >-------

        private void insertParent()
        {
            TabItem t1 = tabControl.SelectedItem as TabItem;

            if (t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false &&t1.Header.ToString().Equals("View Metadata") == false)
                DirList.Items.Insert(0, "..");
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("View Metadata") == false)
                CheckOutDirList.Items.Insert(0, "..");
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false)
                MetadataDirList.Items.Insert(0, "..");
            if (isAddChildOnTop == true)
            {
                childBrowseWindow.DirList.Items.Insert(0, "..");
            }

        }

        //----< function to clear file section on all tabs >-------

        private void clearFiles()
        {
            TabItem t1 = tabControl.SelectedItem as TabItem;

            if (t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false && t1.Header.ToString().Equals("View Metadata") == false)
                FileList.Items.Clear();
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("View Metadata") == false)
                CheckOutFileList.Items.Clear();
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false)
                MetadataFileList.Items.Clear();
            if(isAddChildOnTop == true)
            {
                childBrowseWindow.FileList.Items.Clear();
            }
        }

        //----< function to fill file section on any tab >-------

        private void addFile(string file)
        {
            TabItem t1 = tabControl.SelectedItem as TabItem;

            if (t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false && t1.Header.ToString().Equals("View Metadata") == false)
                FileList.Items.Add(file);
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("View Metadata") == false)
                CheckOutFileList.Items.Add(file);
            if (t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false)

                MetadataFileList.Items.Add(file);
            if (isAddChildOnTop == true)
            {
                childBrowseWindow.FileList.Items.Add(file);
            }

        }
        //----< load getDirs processing into dispatcher dictionary >-------

        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirs", getDirs);
        }
        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFiles", getFiles);
        }
        //----< function to opne browse in pop up window >------

        private void AddChild_Browse_Button(object sender, RoutedEventArgs e)
        {
            childBrowseWindow = new AddChild();
            isAddChildOnTop = true;
            childBrowseWindow.setSender(endPoint_,serverEndPoint,translater,this);
            childBrowseWindow.ShowDialog();
            isAddChildOnTop = false;

            Console.WriteLine("After dialog");
        }
        //----< function to show files in pop up window >------

        private void showFile(string fileName,string fileContent)
        {
            statusBarText.Items.Insert(0, "File recieved. Opening file content...");
            CodePopupWindow p = new CodePopupWindow();
            p.Title = fileName;
            p.codeLabel.Text = fileContent;
            p.Show();
            
        }

        //----< load getFiles processing into dispatcher dictionary >------
        private void DispatcherLoadGetFile()
        {
            Action<CsMessage> getFile = (CsMessage rcvMsg) =>
            {
                Action displayFile = () =>
                {
                    string szFileName = rcvMsg.value("fileName");
                    string szFileContent = System.IO.File.ReadAllText("../../../../CppCommWithFileXfer/SaveFiles" + "/" + szFileName);
                    TabItem t1 = tabControl.SelectedItem as TabItem;

                    if (t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Check-Out") == false && t1.Header.ToString().Equals("View Metadata") == false)

                        showFile(szFileName, szFileContent);
                    if (t1.Header.ToString().Equals("Check-In") == false && t1.Header.ToString().Equals("Browse") == false && t1.Header.ToString().Equals("View Metadata") == false)
                        statusBarText.Items.Insert(0, "File checked out at location :- ../../../../CppCommWithFileXfer/SaveFiles/");
                    
                        if (t1.Header.ToString().Equals("Check-Out") == true )

                    {
                        if (rcvMsg.value("isChildRequest").Equals("false") == true)
                        {
                            int count = Int32.Parse(rcvMsg.value("childCount"));
                            for (int i = 0; i < count; i++)
                            {
                                CsMessage msg = new CsMessage();
                                msg.add("to", CsEndPoint.toString(serverEndPoint));
                                msg.add("from", CsEndPoint.toString(endPoint_));
                                msg.add("command", "getFile");
                                msg.add("isChildRequest", "true");

                                statusBarText.Items.Insert(0, "Requesting checkout of file :- " + (string)CheckOutFileList.SelectedItem);
                                msg.add("path", rcvMsg.value("child" + i.ToString()));
                                translater.postMessage(msg);

                            }
                        }
                    }

                };
                Dispatcher.Invoke(displayFile, new Object[] {  });

            };
            addClientProc("fileSent", getFile);
        }
        private void getAuthorResp()
        {
            Action<CsMessage> authorResp = (CsMessage rcvMsg) =>
            {
                Action displayResp = () =>
                {
                    if (rcvMsg.value("isValid") == "true")
                    {
                        addFilesToList();                    }
                    else
                    {
                        statusBarText.Items.Insert(0,"Invalid Author Name.Please select a valid Author.");
                    }

                };
                Dispatcher.Invoke(displayResp, new Object[] { });

            };
            addClientProc("getAuthor", authorResp);
        }
        public void addFilesToList()
        {
            CsMessage msgNew = new CsMessage();
            msgNew.add("to", CsEndPoint.toString(serverEndPoint));
            msgNew.add("from", CsEndPoint.toString(endPoint_));
            msgNew.add("command", "checkinDone");
            {
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "checkin");
                msg.add("description", DescriptionTextBlock.Text);
                msg.add("checkinStatus", CheckinStatusTextBlock.Text);
                msg.add("category", CategoryTextBlock.Text);
                msg.add("author", AuthorTextBox.Text);
                msg.add("namespace", NamespaceTextBlock.Text);
                String child = ChildListBox.Items[0].ToString();


                for (int j = 0; j < ChildListBox.Items.Count; j++)
                {
                    msg.add("child" + j.ToString(), ChildListBox.Items[j].ToString());
                    if (j > 0)
                        child = child + "," + ChildListBox.Items[j].ToString();
                }
                list.Add(new DataObject() { A = CheckinFileList.Text, B = CheckinStatusTextBlock.Text, C = AuthorTextBox.Text, D = CategoryTextBlock.Text, E = child, F = DescriptionTextBlock.Text, G = NamespaceTextBlock.Text });
                this.dataGrid1.ItemsSource = list;

                msg.add("childCount", ChildListBox.Items.Count.ToString());
                string fileName = System.IO.Path.GetFileName(CheckinFileList.Text);
                System.IO.File.Copy(CheckinFileList.Text, "../../../../CppCommWithFileXfer/ClientSendFiles/" + fileName, true);
                msg.add("file", fileName);
                messageList.Add(msg);
            }
            CheckinFileList.Clear();
            ChildListBox.Items.Clear();
            CheckinStatusTextBlock.Clear();
            DescriptionTextBlock.Clear();
            AuthorTextBox.Clear();
            CategoryTextBlock.Clear();
            NamespaceTextBlock.Clear();

        }
        //----< function to hnadle getChildren response >------

        private void ChildrenResponse()
        {
            Action<CsMessage> childResp = (CsMessage rcvMsg) =>
            {
                Action displayResp = () =>
                {
                    if(rcvMsg.value("DependencyStatus")== "Present")
                    {
                        childBrowseWindow.showAddChildResp(true);
                    }
                    else
                    {
                        childBrowseWindow.showAddChildResp(false);
                }

                };
                Dispatcher.Invoke(displayResp, new Object[] { });

            };
            addClientProc("getChildResp", childResp);
        }

        //----< strip off name of first part of path >---------------------

        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }
        //----< respond to mouse double-click on dir name >----------------

        private void checkoutFileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            // build path for selected dir
            string selectedFile = checkoutPathStack_.Peek() + "/" + (string)CheckOutFileList.SelectedItem;

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getFile");
            msg.add("isChildRequest", "false");

            statusBarText.Items.Insert(0, "Requesting checkout of file :- " + (string)CheckOutFileList.SelectedItem);
            msg.add("path", selectedFile);
            translater.postMessage(msg);

        }

        //----< respond to mouse double-click on dir name >----------------

        private void MetadataList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            string selectedFile = metadataPathStack_.Peek() + "/" + (string)MetadataFileList.SelectedItem;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getMetadata");
            statusBarText.Items.Insert(0, "Requesting Metadata of file :- " + (string)CheckOutFileList.SelectedItem);
            msg.add("path", selectedFile);
            msg.add("Name", (string)MetadataFileList.SelectedItem);
            translater.postMessage(msg);
        }

            //----< respond to mouse double-click on file name >----------------

            private void FileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            // build path for selected dir
            string selectedFile = pathStack_.Peek()+ "/" + (string)FileList.SelectedItem;

            statusBarText.Items.Insert(0, "Requesting to Browse and open file :- " + FileList.SelectedItem);
          
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getFile");
            msg.add("isChildRequest", "false");

            msg.add("path", selectedFile);
            translater.postMessage(msg);
        }

        //----< respond to mouse double-click on dir name >----------------

        private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            TabItem t1 = tabControl.SelectedItem as TabItem;
            string selectedDir = "";
            if (t1.Header.ToString().Equals("Check-In") == false)
                 selectedDir = (string)DirList.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_.Count > 1 || checkinPathStack_.Count >1)  // don't pop off "Storage"
                {
                    if (t1.Header.ToString().Equals("Check-In") == false)
                        pathStack_.Pop();
                    if (t1.Header.ToString().Equals("Browse") == false)
                        checkinPathStack_.Pop();
                }
                else
                    return;
            }
            else{
                if (t1.Header.ToString().Equals("Check-In") == false){
                    path = pathStack_.Peek() + "/" + selectedDir;
                    pathStack_.Push(path);
                }
                if (t1.Header.ToString().Equals("Browse") == false){
                    path = checkinPathStack_.Peek() + "/" + selectedDir;
                    checkinPathStack_.Push(path);
                }
            }
            if (t1.Header.ToString().Equals("Check-In") == false)
                PathTextBlock.Text = removeFirstDir(pathStack_.Peek());
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            if (t1.Header.ToString().Equals("Check-In") == false)
                msg.add("path", pathStack_.Peek());
            if (t1.Header.ToString().Equals("Browse") == false)
                msg.add("path", checkinPathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }

        //----< respond to mouse double-click on dir name >----------------

        private void CheckoutDirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            // build path for selected dir
            TabItem t1 = tabControl.SelectedItem as TabItem;

            string selectedDir = "";
            selectedDir = (string)CheckOutDirList.SelectedItem;

            string path;
            if (selectedDir == "..")
            {
                if (checkoutPathStack_.Count > 1)  // don't pop off "Storage"
                {
                    checkoutPathStack_.Pop();
                }
                else
                    return;
            }
            else
            {
                path = checkoutPathStack_.Peek() + "/" + selectedDir;
                checkoutPathStack_.Push(path);
            }
            // display path in Dir TextBlcok

            CheckOutPathTextBlock.Text = removeFirstDir(checkoutPathStack_.Peek());

            // build message to get dirs and post it
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", checkoutPathStack_.Peek());

            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }

        //----< respond to mouse double-click on dir name >----------------

        private void MetadatatDirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            // build path for selected dir

            string selectedDir = "";
            selectedDir = (string)MetadataDirList.SelectedItem;

            string path;
            if (selectedDir == "..")
            {
                if (metadataPathStack_.Count > 1)  // don't pop off "Storage"
                {
                    metadataPathStack_.Pop();
                }
                else
                    return;
            }
            else
            {
                {
                    path = metadataPathStack_.Peek() + "/" + selectedDir;
                    metadataPathStack_.Push(path);
                }
            }
            // display path in Dir TextBlcok

            MetadataPathTextBlock.Text = removeFirstDir(metadataPathStack_.Peek());

            // build message to get dirs and post it
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", metadataPathStack_.Peek());

            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }

        //----< process incoming messages on child thread >----------------

        private void processMessages()
        {
            ThreadStart thrdProc = () => {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    string msgId = msg.value("command");
                    if(msgId== "quitClient")
                    {
                        translater.stopEndpoint();
                        break;
                    }
                    if (dispatcher_.ContainsKey(msgId))
                        dispatcher_[msgId].Invoke(msg);
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }


        // -----< Send button click event handler >-----------------------------------
        private void Connect_Button_Click(object sender, RoutedEventArgs e)
        { 
            DispatcherConnectToServer();DispatcherLoadGetDirs(); ChildrenResponse();
            DispatcherCheckinResponse();DispatcherMetadataResponse();
            DispatcherLoadGetFile();  DispatcherLoadGetFiles();
            if (txtMsgClientPort.Text.Trim() == "" || txtMsgClientName.Text.Trim() == ""){
                statusBarText.Items.Insert(0,"Please enter a valid input");
            }
            else{
                bool isPortValid = true;
                for (int i = 0; i < txtMsgClientPort.Text.Length; i++)
                {
                    if (!char.IsNumber(txtMsgClientPort.Text[i]))
                    {
                        statusBarText.Items.Insert(0, "Please enter a valid Port number");
                        txtMsgClientPort.Text = "";
                        isPortValid = false;
                        break;
                    }
                }
                if (isPortValid == true){
                    endPoint_ = new CsEndPoint();
                    endPoint_.machineAddress = txtMsgClientName.Text;
                    endPoint_.port = Int32.Parse(txtMsgClientPort.Text);
                    statusBarText.Items.Insert(0, "Requesting Server 8080 for connection.. ");
                    translater = new Translater();
                    bool isStarted = translater.listen(endPoint_);
                    if (!isStarted)
                    {
                        statusBarText.Items.Insert(0, "Port "+ txtMsgClientPort.Text+" in use");
                    }
                    else
                    {
                        connectButton.IsEnabled = false;
                        disconnectButton.IsEnabled = true;
                        processMessages();
                        serverEndPoint = new CsEndPoint();
                        serverEndPoint.machineAddress = "localhost";
                        serverEndPoint.port = 8080;
                        CsMessage msg = new CsMessage();
                        msg.add("to", CsEndPoint.toString(serverEndPoint));
                        msg.add("from", CsEndPoint.toString(endPoint_));
                        msg.add("command", "estConnection");
                        translater.postMessage(msg);
                    }
                }
            }
        }
        private void checkAuthorOfFile()
        {
            CsMessage msgNew = new CsMessage();
            msgNew.add("to", CsEndPoint.toString(serverEndPoint));
            msgNew.add("from", CsEndPoint.toString(endPoint_));
            msgNew.add("command", "getAuthor");
            msgNew.add("name",AuthorTextBox.Text);
            string fileName = System.IO.Path.GetFileName(CheckinFileList.Text);

            msgNew.add("key", NamespaceTextBlock.Text+"::"+ fileName);
            translater.postMessage(msgNew);
        }

        // -----< checkin button click event handler >-----------------------------------

        private void Checkin_Button_Clicked(object sender, RoutedEventArgs e)
        {
            if(NamespaceTextBlock.Text != ""  && CategoryTextBlock.Text != "" && DescriptionTextBlock.Text != "" && AuthorTextBox.Text!= "" && CheckinFileList.Text != ""   && CheckinStatusTextBlock.Text != "")
            {
                checkAuthorOfFile();
            }
            else
            {
                statusBarText.Items.Insert(0,"Invalid Input. Please select all input fields");
            }


        }
        // -----< browse button click event handler >-----------------------------------

        private void Browse_Button_clicked(object sender, RoutedEventArgs e)
        {
            openFileDialog = new OpenFileDialog();
            openFileDialog.ShowDialog();
            string FileName = openFileDialog.FileName;

            CheckinFileList.Text = FileName;
        }
        // -----< delay  >-----------------------------------


        public async Task setDelay(int i)
        {
            int microSec = i * 1000;
            await Task.Delay(microSec);
        }
        // -----< test stub  >-----------------------------------

        public async void testStub(string port)
        {
            statusBarText.Items.Insert(0, "Automation of test cases begins...");
            this.Title = "client-" + port;
            txtMsgClientPort.Text = port;
            await setDelay(4);
            RoutedEventArgs e = new RoutedEventArgs();
            Console.WriteLine("***********Demonstrating requiremnt 1****************");
            Console.WriteLine("***********Used Visual Studio 2017 and the standard C++ libraries,C# the .Net Windows Presentation Foundation framework, and C++ or CLI for the graphical part of each Client****************");
            Console.WriteLine("***********Demonstrating requiremnt 2****************");
            Console.WriteLine("***********Repository Server that provides functionality to check-in, check-out, and browse packages, specified by NoSql database queries****************");
            Console.WriteLine("***********1)Connecting to Server****************");
            Connect_Button_Click(this,e);
            await setDelay(4);tabControl.SelectedIndex = 1;
            Console.WriteLine("\n***********2)Checkin File to Server****************");
            Console.WriteLine("Client Source File::Project3HelpWPF/Translater/Translater.cpp");
            Console.WriteLine("Check in to Server Location::Storage/Translater/Translater.cpp");
            CheckinFileList.Text = "../../../../Translater/Translater.cpp";
            ChildListBox.Items.Insert(0, "../Storage/Process/Process.h.2");
            CheckinStatusTextBlock.Text = "Open";
            DescriptionTextBlock.Text = "This is a source file of translater";
            AuthorTextBox.Text = "Author1";
            CategoryTextBlock.Text = "Category4";
            NamespaceTextBlock.Text = "Translater";

            await setDelay(4);
            Checkin_Button_Clicked(this, null);
            await setDelay(2);
            teststub1();




        }
        //automation continue

        public async void teststub1()
        {
            CheckinFileList.Text = "../../../../Translater/Translater.h";
            ChildListBox.Items.Insert(0, "../Storage/Process/Process.h.1");
            CheckinStatusTextBlock.Text = "Open";
            DescriptionTextBlock.Text = "This is a header file of translater";
            AuthorTextBox.Text = "Author1";
            CategoryTextBlock.Text = "Category4,Category5";
            NamespaceTextBlock.Text = "Translater";
            Checkin_Button_Clicked(this, null);
            await setDelay(2);


            Uploading_Button_Clicked(this, null);
            Console.WriteLine("\n***********3)Checkin Fails due to circular dependency between Process.h.2 and translator.cpp.1****************");
            CheckinFileList.Text = "../../../../Project2/Process/Process.h";
            CheckinStatusTextBlock.Text = "Open";
            DescriptionTextBlock.Text = "This is a processing file";
            AuthorTextBox.Text = "AuthorNew";
            CategoryTextBlock.Text = "Category4,Category5";
            NamespaceTextBlock.Text = "Process";

            await setDelay(4);
            childBrowseWindow = new AddChild();
            isAddChildOnTop = true;
            childBrowseWindow.setSender(endPoint_, serverEndPoint, translater, this);
            childBrowseWindow.automate();

            childBrowseWindow.ShowDialog();
            isAddChildOnTop = false;
            await setDelay(4);
            testStub3();


        }
        //automation continue

        public async void testStub3()
        {
            Console.WriteLine("\n***********4)Checkin Fails due to author validation fails****************");

            childBrowseWindow = new AddChild();
            isAddChildOnTop = true;
            childBrowseWindow.setSender(endPoint_, serverEndPoint, translater, this);
            childBrowseWindow.automateSuccess();

            childBrowseWindow.ShowDialog();
            isAddChildOnTop = false;
            await setDelay(4);

            Checkin_Button_Clicked(this, null);


            await setDelay(4);
            Console.WriteLine("\n***********4)Checking out file with its dependencies****************");

            tabControl.SelectedIndex = 2;
            CheckOutDirList.SelectedIndex = 8;
            await setDelay(4);

            CheckoutDirList_MouseDoubleClick(this, null);
            await setDelay(4);

            CheckOutFileList.SelectedIndex = 1;
            await setDelay(4);

            checkoutFileList_MouseDoubleClick(this, null);
            await setDelay(4);
            Console.WriteLine("\n***********5)Checking out file without dependencies****************");

            CheckOutDirList.SelectedIndex = 0;
            CheckoutDirList_MouseDoubleClick(this, null);
            await setDelay(4);

            CheckOutDirList.SelectedIndex = 2;

            await setDelay(4);

            CheckoutDirList_MouseDoubleClick(this, null); CheckOutFileList.SelectedIndex = 1;

            await setDelay(4);
            CheckOutFileList.SelectedIndex = 0;

            checkoutFileList_MouseDoubleClick(this, null);
            await setDelay(4);
            teststub4();



        }
        //automation continue

        public async void teststub4()
        {
            Console.WriteLine("\n***********5)Browsing of file****************");

            tabControl.SelectedIndex = 3;
            await setDelay(4);

            DirList.SelectedIndex = 1;
            await setDelay(4);

            DirList_MouseDoubleClick(this, null);
            await setDelay(4);

            FileList.SelectedIndex = 1;
            await setDelay(4);

            FileList_MouseDoubleClick(this, null);
            await setDelay(4);

            Console.WriteLine("\n***********5)Querying based on Category****************");

            tabControl.SelectedIndex = 4; txt_browse_category.Text = "Category1;Category4";
            //txt_browse_version.Text = "1";
            await setDelay(2);
            Search_button_clicked(this, null);
            await setDelay(2);
            txt_browse_category.Text = "";
            txt_browse_filename.Text = "Process::Process.h";
            await setDelay(2);
            Search_button_clicked(this, null);
            await setDelay(2);
            Console.WriteLine("\n***********5)Querying based on Category1 or Category4 and version****************");

            txt_browse_version.Text = "1";
            await setDelay(2);
            Search_button_clicked(this, null);
            await setDelay(2);
            txt_browse_filename.Text = "";

            txt_browse_version.Text = "";
            teststub5();
        }
        //automation continue
        public async void teststub5()
        {
            Console.WriteLine("\n***********5)Querying based on Dependency****************");

            txt_browse_dependency.Text = "CheckIn::CheckIn.cpp.2";
            await setDelay(2);

            Search_button_clicked(this, null);
            await setDelay(2);
            Console.WriteLine("\n***********6)Querying for parent not involved in ay dependency****************");

            idp_search_button_clicked(this, null);
            await setDelay(2);

            Console.WriteLine("\n***********7)Display MetaData from database****************");
            tabControl.SelectedIndex = 5;
            MetadataDirList.SelectedIndex = 1;
            await setDelay(2);

            MetadatatDirList_MouseDoubleClick(this, null);
            MetadataFileList.SelectedIndex = 1;
            await setDelay(2);

            MetadataList_MouseDoubleClick(this, null);
            await setDelay(2);

        }
        // -----< disconnect button click event handler >-----------------------------------
        private void Disconnect_Button_Click(object sender, RoutedEventArgs e)
        {
            connectButton.IsEnabled = true;
            disconnectButton.IsEnabled = false;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(endPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "quitClient");

            txtMsgClientPort.Clear();
            txtMsgClientName.Clear();
            pathStack_.Clear();
            checkinPathStack_.Clear();
            checkoutPathStack_.Clear();
            clearFiles();
            clearDirs();
            labelConnectionStatus.Content = " DisConnected";
            for (int i = 1; i < 6; i++)
                {
                    var tab = tabControl.Items[i] as TabItem;
                    tab.IsEnabled = false;
                }
            translater.postMessage(msg);
            statusBarText.Items.Insert(0, "Disconnected");
         }
        // -----< open file button click event handler >-----------------------------------

        private void browseListBoxButtonClick(object sender, RoutedEventArgs e)
        {
            string selectedFile = "../Storage/"+ Browse_FileList.SelectedItem.ToString().Replace("::","/");

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getFile");
            msg.add("isChildRequest", "false");

            statusBarText.Items.Insert(0, "Requesting checkout of file :- " + Browse_FileList.SelectedItem.ToString());
            msg.add("path", selectedFile);
            translater.postMessage(msg);

        }
        // -----< button to upload file to server >-----------------------------------

        private void Uploading_Button_Clicked(object sender, RoutedEventArgs e)
        {
            CsMessage msgNew = new CsMessage();
            for (int i = 0; i < messageList.Count; i++)
            {
                translater.postMessage(messageList[i]);
                //translater.postMessage(msgNew);
                msgNew = messageList[i];
                msgNew.add("fileName", messageList[i].value("file"));

                msgNew.remove("command");
                msgNew.remove("file");

                msgNew.add("command", "checkinDone");
                translater.postMessage(msgNew);
            }
            messageList.Clear();
            list.Clear();
            
        }
        //----< load reply_categoryByBrowse processing into dispatcher dictionary >------
        private void BrowseFileReply()
        {
            Action<CsMessage> browseReply = (CsMessage rcvMsg) =>
            {
                Action<CsMessage> displayFileList = (CsMessage msgcontent) =>
                {
                    var enumer = rcvMsg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {
                        string key = enumer.Current.Key;
                        if (key.Contains("file"))
                        {
                                Browse_FileList.Items.Add(enumer.Current.Value);
                        }
                        else if (key.Contains("NoFile"))
                        {
                            if (rcvMsg.value("tabname") == "browse")
                            {
                                MessageBoxResult result = MessageBox.Show("Sorry No Results , Change Filter Criteria. ", "Results ..", MessageBoxButton.OK, MessageBoxImage.Error);
                            }
                        }
                    }
                   
                };
                Dispatcher.Invoke(displayFileList, new Object[] { rcvMsg }); //invoking delegate

            };
            addClientProc("BrowseReply", browseReply); 

        }
        // -----< open hnadler  to search for files >-----------------------------------

        private void Search_button_clicked(object sender, RoutedEventArgs e)
        {
            Browse_FileList.Items.Clear();
            CsMessage msg = new CsMessage();
            int flag = 0;
            string text;
            text = "Client -> Asking server for files having filters - ";
            if (txt_browse_category.Text != "")
            {
                text = text + "Category (" + txt_browse_category.Text + ") ";
                msg.add("category", txt_browse_category.Text);
                flag = 1;
            }
            if (txt_browse_filename.Text != "")
            {
                text = text + "Filename (" + txt_browse_filename.Text + ") ";
                msg.add("filename", txt_browse_filename.Text);
                flag = 1;
            }
            if (txt_browse_dependency.Text != "")
            {
                text = text + "Dependency (" + txt_browse_dependency.Text + ") ";
                msg.add("dependency", txt_browse_dependency.Text);
                flag = 1;
            }
            if (txt_browse_version.Text != "")
            {
                text = text + "Version (" + txt_browse_version.Text + ") ";
                msg.add("version", txt_browse_version.Text);
                flag = 1;
            }
            if (flag == 1)
            {
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "BrowseReq");
                translater.postMessage(msg); //posting it to client senders queue
            }
            else
            {
                MessageBoxResult result = MessageBox.Show("You have not selected any filter !!", "Browse..", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        // -----< open handler  to searchfor dependecy independent files >-----------------------------------

        private void idp_search_button_clicked(object sender, RoutedEventArgs e)
        {
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getIndependentFile");
            translater.postMessage(msg); //posting it to client senders queue


        }
    }
}
