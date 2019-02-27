/////////////////////////////////////////////////////////////////////////////
// CodePopuoWindow.xaml.cs : Popup window class//
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
 *  This file is used to open a filw ina notepad  
 *  
 *  Public Interface: N/A
 *  
 *  Required Files:
 * =================
 *  MainWindow.xaml MainWindow.xaml.cs Translater.dll 
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

namespace WpfAppClient
{
    /// <summary>
    /// Interaction logic for CodePopupWindow.xaml
    /// </summary>
    public partial class CodePopupWindow : Window
    {
        public CodePopupWindow()
        {
            InitializeComponent();
        }
    }
}
