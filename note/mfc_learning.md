

# MFC基础知识

UI 框架不是线程安全的，所有逻辑应该使用通知的方式，在主线程中更新 UI。

## Windows SDK

系统头文件: window.h, 程序入口: WinMain

- 图标句柄：HICON
- 光标句柄：HCURSOR
- 画刷句柄: HBRUSH

## windows 消息处理机制

![MFC消息队列](./resources/mfc_消息与消息队列.png)

## WIN32 创建窗口

1. 设计窗口 WNDCLASS wc 
2. 注册窗口 RegisterClass
3. 创建窗口 createWindow
4. 显示和更新 showWindow  updateWindow
5. 通过循环取消息  MSG msg  
5.1. 写循环 while（1）  
5.2. GetMessage == false 退出循环   
5.3. 翻译消息  
5.4. 分发消息  
6. 窗口过程  
6.1. LRESULT CALLBACK WindowProc  
6.2. 返回默认处理  
6.3. return DefWindowProc(hwnd, uMsg, wParam, lParam);  
6.4. 点击叉子 WM_CLOSE  destroy  
6.5. WM_DESTROY  postQuitMessage（0）  
6.6. 鼠标左键按下  
6.7. 键盘按下  
6.8. 绘图 文字  

## MFC 创建窗口

1. mfc头文件 afxwin.h
2. 自定义类 继承与 CWinApp  应用程序类  MyApp app 应用程序对象 ，有且仅有一个
3. 程序入口  virutal BOOL InitInstance();
4. 入口里 创建窗口
5. 窗口类 MyFrame 继承与 CFrameWnd 
6. MyFrame 构造中 Create（NULL，标题名称）
7. 创建窗口对象 
8. 显示和更新
9. m_pMainWnd = frame; //保存指向应用程序的主窗口的指针
10. return TRUE
11. 对项目进行配置  在共享DLL中使用MFC

## MFC 向导创建窗口
1. view视类  相片  MainFram类 相框 
2. OnCreate  Create  WM_Create 联系
3. OnDraw OnPaint 如果同时存在 OPaint会覆盖OnDraw

# 对话框与控件

对话框继承于 CDialogEx， 使用 DoModal 创建模态对话框，模态对话框会以阻塞的方式运行。

非模态对话框在 OnInitDialog 中使用Create（对话框ID）创建对话框, 使用 	ShowWindow(SW_SHOWNORMAL)显示以非阻塞的方式显示对话框。

## 模态对话框与非模态对话框

### 模态对话框
1. 设计对话框
```
资源视图
    -> Dialog
        -> 右击
            -> 插入 Dialog

```
2. 修改对话框ID
```
选中对应的Dialog 右击
    -> 属性
        -> ID
```

3. 给对话框添加类
```
点击对话框的模版
    -> 右击
        -> 添加类
            -> 输入类名
                -> 完成
```

4. 添加一个按钮，点击按钮时，会调用回调函数， 在回调函数中实例化对应的对话框类，使用 DoModal 创建模态对话框。

```
    dlg.DoModal();
```

### 非模态对话框：

非模态对话框与模态对话框 1， 2， 3 的步骤是一样的， 在 4 实例化的时候，由于非模态对话框会以非阻塞的方式运行，所以一般是作为唤醒类的成员函数存在，在唤醒类的OnInitDialog 函数创建，在点击按钮的回调函数时显示。

```
OnInitDialog:
    m_dialogShow.Create(IDD_DIALOG_SHOW); // 创建对话框

OnBnClickedButtonShow：
    m_dialogShow.ShowWindow(SW_SHOWNORMAL); // 显示非模态对话框
```

## 静态文本控件

基于MFC对话框，静态文本控件在代码中是一个 CStatic 变量

1. 设计文本控件
```
资源视图
    点击基础对话框
        -> 工具栏
            -> Static Text
                -> 设计文本控件的内容
```

2. 修改ID， 如果文本控件ID以 STATIC 结尾，这个文本控件内容不能更改
```
点击文本控件
    -> 右击
        -> 属性
            -> ID
```

3. 添加变量
```
点击文本控件
    -> 右击
        -> 添加变量
            -> 输入变量名
                -> 完成
```

4. 使用变量
```
setWindowTextW();   // 设置文本控件内容
getWindowTextW();   // 获取文本控件内容

#define HBMP(filepath,width,height) (HBITMAP)LoadImage(AfxGetInstanceHandle(),filepath,IMAGE_BITMAP,width,height,LR_LOADFROMFILE|LR_CREATEDIBSECTION)
m_pic.SetBitmap(HBMP(TEXT("./1.bmp"), rect.Width(), rect.Height())); // 文本控件显示图片
```

5. 显示图片

```
OnInitDialog:
    m_pic.ModifyStyle(0xf, SS_BITMAP | SS_CENTERIMAGE); // 文本控件使用 bitmap 显示

    CRect rect;
    m_pic.GetWindowRect(&rect);                         // 获取文本控件的大小

    #define HBMP(filepath,width,height) (HBITMAP)LoadImage(AfxGetInstanceHandle(),filepath,IMAGE_BITMAP,width,height,LR_LOADFROMFILE|LR_CREATEDIBSECTION)
    m_pic.SetBitmap(HBMP(TEXT("1.bmp"), rect.Width(), rect.Height())); // 显示 bitmap 
```

6. 杂项
```
m_btn.setWindowTextW(); // 改变按钮的文本
m_btn.getWindowTextW(); // 获取按钮的文本
m_btn.EnableWindow(False); // 按钮置灰，不可点击使用
```

## 编辑框

基于MFC对话框，静态文本控件在代码中是一个 CEdit 变量， 编辑框控件基本流程和其他控件一样，这里不详细描述，主要介绍边界框的常用属性。

1. 设计编辑框
2. 修改ID
3. 添加变量(contrl)
4. 修改编辑框属性
```
行为:
    Multiline(多行): True           // 一个编辑框可以输入多行
    want return(想要返回): True     // 接受回车(否则回车会退出编辑框)

外观：
    Auto HScroll: True              // 横向超过边界自动增长
    Auto VScroll: True              // 纵向超过边界自动增长

    Vertical Scroll(垂直滚动)：True     // 垂直滚动条
    Horizontal Scroll(水平滚动)：True   // 水平滚动条
```

5. 退出当前对话框
```
exit(0);                // 退出程序， 所有都退出
CDialog::OnOK();		// 点击确定来退出
CDialog::OnCancel();	// 点击取消来退出
```

6. 添加变量(值)
```
点击文本控件
    -> 右击
        -> 添加变量
            -> 输入变量名
            -> 类别由 控件 改成值
                -> 完成
```
使用控件变量会增加一个CEdit 变量，使用值变量会增加CString 变量，变量会通过使用 UpdateData 更新值到控件中。

```
// 通过 值变量 设置控件
m_edit3 = TEXT("设置值成功");
UpdateData(FALSE);      // 将m_edit3的值同步到控件中

// 通过 值变量 获取控件
UpdateData(TRUE);       // 将控件的值同步到 m_dit3 中
MessageBox(m_edit3);
```

7. 杂项  
查看ui中生成的所有变量
```
右键
    -> 类向导
        -> 成员变量
```

## 下拉框

下拉框属于 Combo Box 控件，下拉框常用属性：

```
行为:
    data(数据)：可乐; 雪碧; 美年达 // 设置下拉框的数据，以;作为分隔符
    sort(排序)： false            // 关闭数据排序

外观：
    type(类型): Dropdown // Dropdown: 下来框可以编辑 Droplist: 下拉框不能编辑
```

常用API:

```
m_cbx.AddString(TEXT("沙僧"));      // 下拉框添加元素

m_cbx.DeleteString(3);              // 下拉框删除对应下标的元素
m_cbx.InsertString(2, TEXT("白龙马")); // 下拉框插入元素到指定下标
m_cbx.SetCurSel(1);                 // 设置下拉框的默认下标
int idx = m_cbx.GetCurSel();        // 获取下拉框的默认下标

CString str;
m_cbx.GetLBText(1, str);	        // 下拉框通过下标获取控件的值

```

## 列表控件
 
列表控件属于 ListControl 控件，列表控件常用属性如下：

```
外观：
    视图(view): report              // 报表模式
```

常用API:
```
OnInitDialog:
    // 插入表头
    CString list_head_str[] = { TEXT("姓名"), TEXT("性别"), TEXT("年龄") , TEXT("标志位")};
    for (int i = 0; i < _countof(list_head_str); i++)
    {
        m_list.InsertColumn(i,list_head_str[i], LVCFMT_CENTER, 60);
    }

    // 插入数据
    m_list.InsertItem(0, TEXT("张三"));
    m_list.SetItemText(0, 1, TEXT("男"));
    m_list.SetItemText(0, 2, TEXT("18"));

    // 设置风格， 整行选中，显示网格
    m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
```

## 树形控件
树形控件属于 TreeControl 控件，树形控件常用属性如下：

```
外观：
    Has Lines(具有线)： True            // 不同层级节点间使用线连接
    Lines At Root(行在根处): True       // 根节点也使用线连接

    Has Buttons(具有按钮): True         // 层级节点之间有折叠按钮
```

添加图标资源:

1. 将图标资源.con文件放到工程res目录下
2. 导入图标资源
```
资源视图
    -> TreeCtrl
        -> Icon
            -> 右键添加资源
                -> 导入
```

常用API
```
OnInitDialog:
    // 0. 加载图标到HICON
    HICON icons[4];
	icons[0] = AfxGetApp()->LoadIconW(IDI_ICON1);
	icons[1] = AfxGetApp()->LoadIconW(IDI_ICON2);
	icons[2] = AfxGetApp()->LoadIconW(IDI_ICON3);
	icons[3] = AfxGetApp()->LoadIconW(IDI_ICON4);

    // 1. 设置图标
	//CImageList m_imgList; // 这个不能放在栈中，否则初始化结束后，内存就销毁了
	m_imgList.Create(30, 30, ILC_COLOR32, 4, 4);
	for (int i = 0; i < 4; i++)
	{
		m_imgList.Add(icons[i]);
	}
	m_tree.SetImageList(&m_imgList, TVSIL_NORMAL); // TVSIL_NORMAL or TVSIL_STATE 

	// 2. 设置节点
	HTREEITEM root = m_tree.InsertItem(TEXT("根节点"), 0, 0, NULL);     // 使用 icons[0]
	HTREEITEM parent = m_tree.InsertItem(TEXT("父节点"), 1, 1, root);
	HTREEITEM sub1 = m_tree.InsertItem(TEXT("子节点1"), 2, 2, parent);
	HTREEITEM sub2 = m_tree.InsertItem(TEXT("子节点2"), 3, 3, parent);

	// 3. 设置默认节点
	m_tree.SelectItem(sub1);

OnTvnSelchangingTree1： // 节点切换事件
	HTREEITEM item = m_tree.GetSelectedItem();      // 获取选中的节点
	CString node_name = m_tree.GetItemText(item);   // 获取选中节点的名字
	OutputDebugString(node_name);
```

## 标签页

标签页控件属于 Tab Control 控件，标签页控件常用属性如下：

添加两个对话框，并修改其属性:
```
外观:
    Border(边框): None      // Dialog Frame 改成None : 不显示边框
    Style(样式): child      // Popup 改为 child, 以子窗口的形式弹出
```

增加 TabSheet 项，标签页添加变量，变量类型为TabSheet,

```
OnInitDialog: 
	m_tab.AddPage(TEXT("系统管理"), &m_dlg1, IDD_DIALOG1);
	m_tab.AddPage(TEXT("系统设置"), &m_dlg2, IDD_DIALOG2);

	m_tab.Show();
```

## TIPS

```
ON_NOTIFY(NM_RCLICK, IDC_LIST_FRAMELIST, &CPicPreviewDlg::OnRclickListFramelist)    // 在 IDC_LIST_FRAMELIST 控件上点击鼠标右键
ON_NOTIFY(NM_CLICK, IDC_LIST_FRAMELIST, &CPicPreviewDlg::OnClickListFramelist)      // 在 IDC_LIST_FRAMELIST 控件上点击鼠标左键
```

### MemDC 保存图片 



# 销售系统

## 新建项目

```
应用程序类型: 单文档
项目类型: MFC标准
MFC使用: 在共享 DLL 中使用MFC

命令行：使用经典菜单
    不使用停靠传统工具栏
```

添加资源后，修改窗口图标和位置
```
CMainFrame::OnCreate：
    // 设置窗口图标
    HICON hIcon = AfxGetApp()->LoadIconW(IDI_ICON_WIN);
    if (hIcon != nullptr) {
        SetClassLongPtr(m_hWnd, GCLP_HICON, (LONG_PTR)hIcon);
        //SetClassLongPtr(m_hWnd, GCLP_HICONSM, (LONG_PTR)hIcon);
    }
    // 设置右标题，左标题在 CSaleSystemDoc::OnNewDocument 中设置
    SetTitle(TEXT("2025-12-08"));

    //设置窗口大小
    MoveWindow(0, 0, 800, 500);

    //设置居中显示
    CenterWindow();
```

# Q&A

1. 报错信息：
```
#error:  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]	mfc	C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.37.32822\atlmfc\include\afx.h	24	
```
A:
```
属性
    -> 配置属性
        -> 高级
            -> MFC的使用
                -> 在共享 DLL 中使用 MFC
```

2. 编辑框例子中，在编辑框输入完后，回车会触发OnOK消息导致对话框退出

重写对话框的OnOK消息

```
类视图
    -> EditCtrl
        -> CEditCtrlDlg 右键
            -> 属性
                -> 重写
                    -> OnOK
                        // CDialogEx::OnOK(); 屏蔽掉退出代码
```

3. Windows GDI 和许多图像格式（如 BMP）要求每一行像素数据在内存中按 4 字节边界对齐.

CImage 分配内存时需要宽度向4对齐。