/////////////////////////////////////////////////////////////////////////////
// AddChild.xaml.cs :This is a browse window for child selection  //
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
 *  This window opens the browse functionalty to facilitate the user to selct the child 
 *  
 *  Public Interface: N/A
 *  
 *  Required Files:
 * =================
 *  MainWindow.xaml MainWindow.xaml.cs Translater.dll AddChild.xaml.css
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
using System.Windows.Shapes;
using System.Threading;
using System.IO;
using Microsoft.Win32;
using MsgPassingCommunication;


namespace WpfAppClient
{
    /// <summary>
    /// Interaction logic for AddChild.xaml
    /// </summary>
    public partial class AddChild : Window
    {

        private Translater translater_;
        private CsEndPoint endPoint_;
        private CsEndPoint serverEndPoint_;
        private MainWindow mainWindow_;



        // -----<window intilization >-----------------------------------

        public AddChild()
        {
            InitializeComponent();
        }
        

        string selectedFile;
        // -----< button click handler in file list >-----------------------------------

        private void FileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            // build path for selected dir
            selectedFile = mainWindow_.addChildPathStack_.Peek() + "/" + (string)FileList.SelectedItem;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getChildren");
            msg.add("path", selectedFile);
            msg.add("PFileKey", mainWindow_.NamespaceTextBlock.Text + "::" + System.IO.Path.GetFileName(mainWindow_.CheckinFileList.Text));
            translater_.postMessage(msg);


        }
        // -----< function to show response>-----------------------------------

        public void showAddChildResp(bool isPresent)
        {
            if (isPresent)
            {
                mainWindow_.statusBarText.Items.Insert(0, "Cannot add child causing Circular dependency. Select a different child.");
            }
            else
            {
                string parent = System.IO.Path.GetFileName(mainWindow_.CheckinFileList.Text);
                string child = selectedFile.Substring(11, selectedFile.Length - 12).Replace("/","::");
                if(parent.Contains(child))
                {
                    mainWindow_.statusBarText.Items.Insert(0, "Cannot add child causing Circular dependency. Select a different child.");

                }
                else
                    mainWindow_.ChildListBox.Items.Insert(0, selectedFile);

            }
            this.Close();

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

        private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            string selectedDir = "";
                selectedDir = (string)DirList.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (mainWindow_.addChildPathStack_.Count > 1)  // don't pop off "Storage"
                {
                    mainWindow_.addChildPathStack_.Pop();
                }
                else
                    return;
            }
            else
            {
                {
                    path = mainWindow_.addChildPathStack_.Peek() + "/" + selectedDir;
                    mainWindow_.addChildPathStack_.Push(path);
                }
               
            }
            PathTextBlock.Text = removeFirstDir(mainWindow_.addChildPathStack_.Peek());
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", mainWindow_.addChildPathStack_.Peek());
            translater_.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater_.postMessage(msg);
        }
        // -----< delay  >-----------------------------------
         async Task Delay(int i)
        {
            int microSec = i * 1000;
            await Task.Delay(microSec);
        }
        // -----< automation code >-----------------------------------

        public async void automate()
        {
            DirList.SelectedIndex = 7;
            await Delay(4);
            DirList_MouseDoubleClick(this,null);
            await Delay(4);

            FileList.SelectedIndex = 0;
            await Delay(4);

            FileList_MouseDoubleClick(this,null);


        }
        // -----< automation code >-----------------------------------

        public async void automateSuccess()
        {
            FileList.SelectedIndex = 1;
            await Delay(4);

            FileList_MouseDoubleClick(this, null);


        }
        // -----<set window intial parameters >-----------------------------------

        public void setSender(CsEndPoint clientEndPoint, CsEndPoint serverEndPoint,Translater translater,MainWindow w)
        {
            endPoint_ = clientEndPoint;
            serverEndPoint_ = serverEndPoint;
            translater_ = translater;
            mainWindow_ = w;
            PathTextBlock.Text = "Storage";

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));

            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", mainWindow_.addChildPathStack_.Peek());
            Console.WriteLine("In set sender6");

            translater_.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater_.postMessage(msg);
            

        }
        
    }
}
