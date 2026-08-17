# Windows TIPS 

## 打印LOG

OutputDebugString： 根据环境选择使用打印宽字符或者窄字符  

```
// 常用打印LOG

OutputDebugString(L"-------- KeyDown Start ---\n");

wchar_t log_buf[1024];
swprintf_s(log_buf, _countof(log_buf), L"assic: wParam = %#04x(%lc), S = %#04x, s = %#04x\n", (int)wParam, (int)wParam, (int)'S', (int)'s');
OutputDebugString(log_buf);

```

## MFC 字符串 CString

```
// 1. MessageBox 和 OutputDebugString 直接使用
CString str;
str.Format(TEXT("CString: nChar = %u(%c), nRepCnt = %u, nFlags = %u"), nChar, nChar, nRepCnt, nFlags);
MessageBox(str);
OutputDebugString(str);

// 2. char 与 CString之间的转换， 借助 CT2CA 工具
const char* pchar = "Hello World";
CString cstr(pchar);	

CT2CA ca(cstr);
char* pchar2 = ca; // 这里需要注意：当CT2CA 内存释放后，pchar2内存也会被释放

// 3. 修改CString
cstr = _T("LC: Hello World"); 

// 一般不使用下面的方法，容易内存越界
TCHAR* cstr_buffer = cstr.GetBuffer(100); // 如果原始缓冲区不够，会自动扩容到100
_tcscpy_s(cstr_buffer, 100, _T("LC--------------LLCCC"));
cstr.ReleaseBuffer();

```

## 查看文件是否存在 
```
BOOL file_access = (_taccess_s(L"1.bmp", 0) == 0);
    
CString log_str;
log_str.Format(TEXT("file_accress: %d\n"), file_access);
OutputDebugString(log_str);
```

## Windows 字符集

多字节编码：一个字符对应一个字节  
宽字节编码: 一个字符对应多个字节, 比如: Unicode编码， utf-8 一个字符对应3个字节，GBK 一个字符对应 2个字节

```
TCHAR buf[1024];        // 多字节 与 宽字节编码自适应
TEXT("Hello, world");   // 多字节 与 宽字节编码自适应

char str[256] = "Hello, world";         // 多字节编码
wchar_t wstr[236] = L"Hello, world";    // 宽字节编码

char str2[256];
wchar_t wstr2[256];

strncpy(str2, _countof(str), str);      // 多字节拷贝, _countof： 计算数组容量大小
wcscpy_s(wstr, _countof(wstr), wstr2);  // 宽字节拷贝

```

# MFC TIPS

| 模块名                  | 说明                     |
|------------------------|--------------------------|
| Windows 应用程序| 顶层应用 |
| MFC | MicroSoft Foundation Classes, 微软基础类库 |
| ATL | Active Template Library, 轻量模板库 |
| DIB | Device-Independent Bitmap，设备无关位图 |
| DDB | 设备有关位图 |
| DC | Memory Device Context，设备上下文，可以使用GDI绘制DC|
| GDI+ | Graphics Device Interface Plus, 图形设备接口Plus|
| GDI | Graphics Device Interface, 图形设备接口 |

---

## Window 坐标系

Window 窗口坐标示意：

```
虚拟屏幕坐标 (Screen)
┌────────────────────────────┐
│                            │
│   Dialog 窗口 (Window)     │
│   ┌────────────────────┐   │
│   │ Dialog Client      │   │
│   │ (0,0)              │   │
│   │                    │   │
│   │   Toolbar 窗口     │   │
│   │   ┌────────────┐  │   │
│   │   │ Client     │  │   │
│   │   │ (0,0)      │  │   │
│   │   └────────────┘  │   │
│   └────────────────────┘   │
└────────────────────────────┘
```

GetClientRect: 获取客户区坐标，原点位于客户区左上角，LT:(0, 0)  
GetWindowRect: 获取窗口坐标(包含客户区与非客户区)，原点位于虚拟屏幕坐标原点，LT可以为负值  

```
CRect client_rc;
m_show.GetClientRect(&client_rc);   // 客户区坐标：基于客户区坐标原点，LT:(0, 0) RB:(w, h)

CRect win_rc;
m_show.GetWindowRect(&win_rc);      // 窗口坐标：基于虚拟屏幕坐标原点，包含控件的客户区与非客户区

CRect parent_client_rc = win_rc;
m_show.GetParent()->ScreenToClient(&parent_client_rc); // 获取当前窗口在 父窗口客户区的坐标

CPoint cursor;
GetCursorPos(&cursor);		        // 获取鼠标点击位置在虚拟屏幕的坐标

```

### API 函数

- ShowWindow(int nCmdShow): 显示窗口
```
#define SW_HIDE             0       // 隐藏窗口
#define SW_SHOWNORMAL       1       // 显示窗口，不会激活它
#define SW_NORMAL           1   
#define SW_SHOWMINIMIZED    2       // 最小化状态显示窗口(先创建，再最小化，出现在任务栏里)
#define SW_SHOWMAXIMIZED    3       // 最大化状态显示窗口
#define SW_MAXIMIZE         3       
#define SW_SHOWNOACTIVATE   4       // 显示窗口，但不激活它
#define SW_SHOW             5       // 显示窗口，并激活它   
#define SW_MINIMIZE         6       // 最小化到任务栏
#define SW_SHOWMINNOACTIVE  7       // 
#define SW_SHOWNA           8
#define SW_RESTORE          9       // 窗口当前处于 最小化/最大化 状态时，恢复回正常状态
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11
```

- SetWindowPos() / MoveWindow(): 设置窗口位置
```
SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 窗口置顶，不改变大小和尺寸
SetWindowPos(NULL, 100, 100, 800, 600, 0);      // 移动窗口到 100, 100
MoveWindow(100, 100, 800, 600);                 // 移动窗口到 100, 100 窗口缩放到 800, 600
```
- CenterWindow(): 设置窗口居中（基于父窗口居中）
- UpdateWindow(): 调用此函数会发送 WM_PAINT 消息。
- Invalidate: 标记窗口为 无效区域， 不会立即重绘该区域，等下次消息触发时会重绘该区域  

## 消息机制

Windows 消息流一般经过如下阶段：

```
硬件
 ↓
Windows 消息队列
 ↓
GetMessage / PeekMessage
 ↓
MFC: PreTranslateMessage   ←（最早拦截点）
 ↓
TranslateMessage
 ↓
DispatchMessage
 ↓
CWnd::WindowProc
 ↓
ON_WM_xxx 消息映射
 ↓
DefWindowProc             ←（最后兜底）
```

对于鼠标和键盘消息，能用消息映射就别用 PreTranslateMessage
能用 PreTranslateMessage 就别碰 DefWindowProc


## Dialog / CWnd

### Dialog 启动顺序

模态对话框的调用顺序：

```
CMyDlg dlg;
dlg.DoModal();  // 模态对话框
```

1. 构造函数 CMyDlg::CMyDlg() 进行成员变量的初始化
2. 创建对话框: CreateDialog / CreateDialogIndirect 触发消息: WM_INITDIALOG 
3. CMyDlg::OnInitDialog() 被调用
4. 模态对话框是不会调用到 CMyDlg::OnCreate 函数, 当定义 ON_WM_CREATE 消息后，会先调用: OnCreate 再调用 OnInitDialog();

非模态对话框的调用顺序:
```
CMyDlg* pDlg = new CMyDlg;               // 1. 构造对象
pDlg->Create(IDD_MYDLG, pParent);        // 2. 创建窗口（关键！）
pDlg->ShowWindow(SW_SHOW);               // 3. 显示窗口
```

1. 构造函数 CMyDlg::CMyDlg()
2. CWnd::CreateEx() 触发消息：WM_CREATE, 调用 OnCreate()
3. 系统发送 WM_INITDIALOG 消息：WM_INITDIALOG, 调用 OnInitDialog()
4. ShowWindow 函数 触发: OnShowWindow()

### 子对话框

子对话框创建：

```
子对话框设计:
    边框：None
    样式：child

子对话框创建:
    AssistDlg assist_dlg_;
    assist_dlg_.Create(IDD_ASSIST_DIALOG, this);
    
    assist_dlg_.MoveWindow(assist_rc);
    assist_dlg_.ShowWindow(SW_SHOW);
```

### 异步线程更新UI

后台线程更新 UI，一定要用自定义消息 + PostMessage

```
// 1. 定义消息映射
BEGIN_MESSAGE_MAP(AssistDlg, CDialogEx)
	ON_MESSAGE(WM_UPDATE_WIDTH, &AssistDlg::OnUpdateWidth)
END_MESSAGE_MAP()

// 2. 定义传输的数据结构
struct WidthInfo {
    int width;
};

// 3. 异步线程制作传输的数据，使用PostMessage发送数据
auto* info = new WidthInfo{ 4096 };
::PostMessage(hAssistDlg, WM_UPDATE_WIDTH, reinterpret_cast<WPARAM>(info), 0);

// 4. 消息处理函数接收数据，更新UI
LRESULT AssistDlg::OnUpdateWidth(WPARAM wParam, LPARAM lParam) {
    std::unique_ptr<WidthInfo> info(reinterpret_cast<WidthInfo*>(wParam)); // 接管所有权，防止忘记 delete

    CString text;
    text.Format(L"%d px", info->width);
    m_imgWidth.SetWindowTextW(text);
}
```

UI 按钮操作耗时长，为避免卡主UI，也需要使用异步线程更新UI的方式。
```
#define WM_AUTOFLIP_DONE		(WM_USER + 40000)

BEGIN_MESSAGE_MAP(CPicPreviewDlg, CDialogEx)
	ON_MESSAGE(WM_AUTOFLIP_DONE, &CPicPreviewDlg::OnAutoFlipDone)
END_MESSAGE_MAP()

UINT AutoFlipThreadProc(LPVOID pParam)
{
	CPicPreviewDlg* pDlg = (CPicPreviewDlg*)pParam;
	if (pDlg) {
		pDlg->m_show.AutoFlip();
		pDlg->PostMessage(WM_AUTOFLIP_DONE, 0, 0); // 处理结束后更新UI按键
	}
	return 0;
}

void CPicPreviewDlg::OnBnClickedButtonAutoflip()
{
	GetDlgItem(IDC_BUTTON_AUTOFLIP)->EnableWindow(FALSE); // 置按钮为不可按下，防止重复触发这个任务
	AfxBeginThread(AutoFlipThreadProc, this);  // 异步线程调用 AutoFlipThreadProc 
}

LRESULT CPicPreviewDlg::OnAutoFlipDone(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_BUTTON_AUTOFLIP)->EnableWindow(TRUE);
	return 0;
}
```

#### Q&A

1. SendMessage 与 PostMessage 使用场景
```
同线程 → SendMessage   
要返回 → SendMessage  
跨线程 → PostMessage  
怕卡死 → PostMessage  
```

### Timer定时器:

#### Timer使用方法：
```
// 1. 定义 定时器消息映射
BEGIN_MESSAGE_MAP(AssistDlg, CDialogEx)
    ON_WM_TIMER()
ON_WM_TIMER()

// 2. 创建定时器
SetTimer(ID, 1000, NULL); // ID: 定时器ID，1000: 1秒，NULL: 窗口句柄，如果为NULL，则为全局定时器

// 3.定时器处理函数
void AssistDlg::OnTimer(UINT_PTR nIDEvent) {

}

// 4. 销毁定时器
KillTimer(ID); // ID: 定时器ID
```
#### Q&A

1. Timer 定时器收不到消息？

- 没有加入定时器的消息映射
- 创建定时器的ID冲突，建议使用枚举变量定义定时器ID
    ```
    enum { TIMER_ELAPSED = 1001 };
    ```
- 定时器窗口隐藏，不能收到WM_TIMER消息
- 窗口还没有创建就启动定时器，定时器创建失败

### Accelerator 加速键



### StringTable 

1. 资源管理器添加 String Table 资源
```// ID, val
ID_TOOLBAR_SAVE   "Save\nSave image" 
```

2. 鼠标悬停显示 Show Tip
```
toolbar_.SetRouteCommandsViaFrame(FALSE); // 默认路由消息是返回给CFrameWnd,对话框没有CFrameWnd这里返回给父窗口，所以设置FALSE
toolbar_.SetShowTooltips(1);			  // 显示启动工具提示 与 CBRS_TOOLTIPS	作用相同，可省略		
toolbar_.EnableTrackingToolTips();		  // 提示框跟随鼠标移动
```

3. 代码根据 ID 加载字符串
```
CString hint;
hint.LoadString(IDS_OVER_EXPOSURE_HINT);
GetDlgItem(IDC_STATIC_HINT)->SetWindowText(hint);
```

## GDI+ 绘图

GDI 在系统启动时就已经初始化好了，但GDI+ 必须要显示的初始化;

```
// GDI+ 显示初始化

BOOL CMyApp::InitInstance() {
    // GDI+ 初始化
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &input, nullptr);
	return TRUE;
}

int CMyApp::ExitInstance() {
    // GDI+ 释放, 类的私有变量: ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}

// GDI+ 绘图
BOOL CAboutDlg::OnInitDialog() {
    // 背景图片
    m_backgroundImg = Gdiplus::Image::FromFile(L"res/about.png"); // Image* m_backgroundImg;

    // 根据图片原始的宽高比，计算窗口的尺寸
	CRect window_rc;
	GetWindowRect(&window_rc);

	CRect client_rc;
	GetClientRect(&client_rc);

	int no_client_width = window_rc.Width() - client_rc.Width();
	int no_client_height = window_rc.Height() - client_rc.Height();

	double scale = (double)img_->GetWidth() / (double)img_->GetHeight();

    // 客户区的尺寸
	int stretch_height = client_rc.Height();
	int stretch_width = stretch_height * scale;

	// 基于虚拟屏幕或者父窗口的坐标，由于AboutDlg没有父窗口，这里基于虚拟屏幕坐标
	CWnd::SetWindowPos(&CWnd::wndBottom, 0, 0,
		stretch_width + no_client_width,    // 客户区 + 非客户区 = 窗口的尺寸
		stretch_height + no_client_height, 0);

    CenterWindow();
}

void CAboutDlg::OnPaint() {
    CPaintDC dc(this);
	Gdiplus::Graphics g(dc);

    if (!m_backgroundImg)
		return;

    // 提高画图质量，但会降低效率
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic); // 位图拉伸使用双三次插值算法
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality); // 线条、圆、多边型平滑
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);	// 像素对齐、防止模糊

	CRect rc;
	GetClientRect(&rc);

    // 1. GDI+ 画背景
	g.DrawImage(m_backgroundImg, 
				RectF(  // 目标的指定区域
					0,
					0,
					(Gdiplus::REAL)rc.Width(),
					(Gdiplus::REAL)rc.Height()
				),
				0, 0,   // 源图的指定区域
				(Gdiplus::REAL)m_backgroundImg->GetWidth(),
				(Gdiplus::REAL)m_backgroundImg->GetHeight(),
				Gdiplus::UnitPixel);

    // 2. GDI+ 写字
	std::wstring text = L"Industrial Negative Scanning And AI-Eval System";
	Gdiplus::RectF bound;

	Gdiplus::FontFamily ff(L"Arial");
	Gdiplus::SolidBrush white(Gdiplus::Color(255, 255, 255)); // 白色
	Gdiplus::SolidBrush black(Gdiplus::Color(0, 0, 0));

	Gdiplus::REAL titleSize = rc.Height() * 0.025f;
	Gdiplus::Font titleFont(&ff, titleSize);

#if 1
	// 画面中间写字  
	Gdiplus::REAL y = rc.Height() * 0.18f;
	g.MeasureString(text.c_str(), (INT)text.size(),
		&titleFont,
		Gdiplus::RectF(0, 0, (REAL)(rc.Width()), 1000),
		nullptr, &bound);

	Gdiplus::REAL x = ((REAL)rc.Width() - bound.Width) / 2; // 计算中间写字时，X的起始位置
	g.DrawString(text.c_str(), (INT)text.size(),
		&titleFont, Gdiplus::PointF(x, y), &white);
#else
	// 设置矩形框内写字
	Gdiplus::RectF layoutRect(
		(REAL)(rc.Width() * 3 / 4),   // X: 起始 X
		0,                            // Y: 起始 Y
		(REAL)(rc.Width() / 4),       // Width: 限制宽度（超过限制宽度会触发换行）
		1000                          // Height: 限制高度 （换行超过限制高度会被截断）
	);
	g.MeasureString(text.c_str(), (INT)text.size(),
		&titleFont,
		layoutRect,
		nullptr, &bound);
	g.DrawString(text.c_str(), (INT)text.size(),
		&titleFont, layoutRect, nullptr, &white);
#endif
}

```

## CImage

|位深	| 像素里存的是什么 |
|-------|----------------|
|8bit	| 索引值（0–255） |
|24bit	| RGB |
|32bit	| RGBA |

### 常用操作 
```
// 1.创建索引图像
CImage* img = new CImage;
img->Create(nW, nH, 8, 0);  // 8: bpp, 0: 

// 是否是索引图像, 8 bpp图像一定是索引图像
if (img->IsIndexed())  {
    // 设置颜色表
	RGBQUAD ColorTable[256];
	for (int i = 0; i < 256; i++)	{
		ColorTable[i].rgbBlue = (BYTE)i;
		ColorTable[i].rgbGreen = (BYTE)i;
		ColorTable[i].rgbRed = (BYTE)i;
		ColorTable[i].rgbReserved = 0;
	}
	img->SetColorTable(0, 256, ColorTable);
}

// 2. 设置像素
BYTE* pixel = (BYTE*)img->GetBits();
int step = img->GetPitch(); // 这里step如果是负数表示图像存储时倒置

for (int y = 0; y < img->GetHeight(); y++) {
    for (int x = 0; x < img->GetWidth(); x++) {
        pixel[y * step + x] = 255;
        // 如果是 32 bpp 的话，存储顺序是 BGRA
        // pixle[y * step + x*4 + 0] = B;
        // pixle[y * step + x*4 + 0] = G;
        // pixle[y * step + x*4 + 0] = R;
        // pixle[y * step + x*4 + 0] = A; // a:0 完全透明，255：完全不透明
        
    }
}

// 3. 保存图像
CString name;
name.Format(TEXT("tmp/seg_%d.png"), i);
HRESULT hr = img->Save(name, Gdiplus::ImageFormatPNG);

// 4. 加载图像
img->load(_TEXT("tmp/seg_%d.png"));

// 5.下采样图像(CImage* src, CImage* dst)
int dst_w = img->GetWidth() / 2;
int dst_h = img->GetHeight() / 2;

CImage* img2 = new CImage;
img2->Create(dst_w, dst_h, 8, 0);

if (img->GetBpp() == 8) {
    RGBQUAD src_palette[256];
    RGBQUAD dst_palette[256];

    src->GetColorTable(0, 256, src_palette);

    memcpy(dst_palette, src_palette, 256 * sizeof(RGBQUAD));
    dst->SetColorTable(0, 256, dst_palette);
}

HDC hdc = dst->GetDC(); 
/**
 * 插值算法：
 * COLORONCOLOR: 直接丢弃 
 * HALFTONE: 双线性/平滑滤波
 * BLACKONWHITE: 打印机
 */
::SetStretchBltMode(hdc, HALFTONE);
// 把src图像拉伸到dst的指定位置:(0, 0) (dst_w， dst_h)
src->StretchBlt(hdc, 0, 0, dst_w, dst_h, SRCCOPY);
dst->ReleaseDC();

// CImage::BitBlt: 1:1 传输到目标区域(源区域可以指定进行裁剪)
/**
 * BOOL BitBlt(
 *    HDC hDestDC,          // 目标DC
 *    int xDest,            // 目标左上坐标x
 *    int yDest,            // 目标左上坐标y
 *    DWORD dwRop = SRCCOPY // 
 * ) const;
 */
/**
 * BOOL BitBlt(
 *    HDC hDestDC,          // 目标DC
 *    int xDest,            // 目标左上坐标x
 *    int yDest,            // 目标左上坐标y
 *    int nDestWidth,       // 复制的宽度（1:1复制，到宽度就截止）
 *    int nDestHeight,      // 复制的高度（1:1复制，到高度就截止）
 *    int xSrc,             // 源左上坐标x
 *    int ySrc,             // 源左上坐标y
 *    DWORD dwRop = SRCCOPY
 * ) const;
 */

// CImage::StretchBlt: 拉伸缩放到目标区域(源区域可以指定进行裁剪)
/**
 * BOOL StretchBlt(
 *    HDC hDestDC,          // 目标HDC
 *    int xDest,            // 目标左上角坐标x
 *    int yDest,            // 目标左上角坐标y
 *    int nDestWidth,       // 目标宽度
 *    int nDestHeight,      // 目标高度
 *    int xSrc = 0,         // 源左上角坐标 x(可以省略，默认源 全图)
 *    int ySrc = 0,         // 源左上角坐标 y
 *    int nSrcWidth = -1,   // 源图像宽度
 *    int nSrcHeight = -1,  // 源图像高度
 *    DWORD dwRop = SRCCOPY
 * ) const;
 */
/**
 * 注: CImage不是零拷贝容器，工程上推荐使用：GetBits + memcpy，如果要极致性能的话可以使用:CreateDIBSection
 */

// 6. Attach  将已创建的 HBITMAP 管理权交给 CImage, CImage析构时会自动调用 DelteteObject(HBITMAP)
// 6.1. 原则上只能 Attach DIB 数据, CreateCompatibleBitmap 创建的是DDB数据,使用 CreateDIBSection 或 CImage::Create() 创建的位图才是 DIB
// 6.2  CImage 调用Create后，就不能Attach了，否则会触发断言错误
CImage.Attach(HBITMAP); // 

```

## HBITMAP

|类名| 说明| 用途 |
|----|----|-------|
| CDC | 设备上下文 | |
| HBITMAP | GDI 位图内核句柄 |
| CBitMap | MFC 对 HBITMAP 的 C++ 封装 | 绘图、双缓冲、内存DC |
| CImage  | 偏文件和图像像素操作的类, 属于ATL库 | 保存png或者访问像素 |


```
// 1. CBitmap -> HBITMAP
CBitmap bmp;
bmp.CreateCompatibleBitmap(&dc, w, h);

HBITMAP hBmp = (HBITMAP)bmp.GetSafeHandle(); // 这里bBmp可以接入 CImage.Attach(hBmp);
// ❗ 不要 DeleteObject(hBmp)

// 2. HBITMAP -> CBitmap
HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h);

CBitmap bmp;
bmp.Attach(hBmp);   // CBitmap 接管, 不需要调用 DeleteObject(hBmp);

// 3. HBITMAP -> CImage
HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h);

CImage img;
img.Attach(hBmp);   // CImage 接管

// 4. CBitmap -> CImage
// 4.1. 转移所有权
CBitmap bmp;
bmp.CreateCompatibleBitmap(&dc, w, h);

HBITMAP hBmp = (HBITMAP)bmp.Detach(); // 断开 bmp 所有权

CImage img;
img.Attach(hBmp);                     // img 接管

// 4.2. 复制
CImage img;
img.Create(w, h, 32);

CDC memDC;
memDC.CreateCompatibleDC(nullptr);
memDC.SelectObject(&bmp);

CDC imgDC;
imgDC.Attach(img.GetDC());

imgDC.BitBlt(0, 0, w, h, &memDC, 0, 0, SRCCOPY);

img.ReleaseDC();

// 5. CImage -> HBITMAP
CImage img;
img.Load(L"test.png");

HBITMAP hBmp = img.Detach();  // 你接管,后续用完后需要调用 DeleteObject(hBmp);

// 6. CImage -> CBitmap
CImage img;
img.Load(L"test.png");

CBitmap bmp;
bmp.Attach(img.Detach());  // 所有权转移
```

## 小工具：

```
// 1. SelectObject后一定要还原，GDI 泄漏、画面异常、随机崩溃
HBITMAP old = (HBITMAP)SelectObject(hdc, hBmp);
// ...
SelectObject(hdc, old);
```

```
1. 保存CDC 的图像
bool SaveMemDCToFile(CDC& memDC, const CString& filePath, int width, int height, CRect* rect = nullptr)
{
    // 1. 创建兼容位图（绑定到位图对象）
    CBitmap bitmap;
    if (!bitmap.CreateCompatibleBitmap(&memDC, width, height))
    {
        return false; // 创建失败
    }

    // 2. 创建临时内存 DC，并选入位图
    CDC tempDC;
    if (!tempDC.CreateCompatibleDC(&memDC))
    {
        return false;
    }
    CBitmap* pOldBitmap = tempDC.SelectObject(&bitmap);

    // 3. 将 memDC 的内容复制到 tempDC（即复制到位图）
    tempDC.BitBlt(0, 0, width, height, &memDC, 0, 0, SRCCOPY);

    if (rect) {
        CBrush* pOldBrush = tempDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
        CPen pen(PS_SOLID, 2, RGB(0, 0, 0)); // 红色边框
        CPen* pOldPen = tempDC.SelectObject(&pen);

        tempDC.Rectangle(rect->left, rect->top, rect->right, rect->bottom);

        // 恢复
        tempDC.SelectObject(pOldBrush);
        tempDC.SelectObject(pOldPen);
    }

    // 4. 获取 HBITMAP 句柄并附加到 CImage
    HBITMAP hBitmap = (HBITMAP)bitmap.GetSafeHandle();
    if (!hBitmap)
    {
        tempDC.SelectObject(pOldBitmap);
        return false;
    }

    CImage image;
    image.Attach(hBitmap); // CImage 接管该句柄（注意：之后不要手动 DeleteObject）

    // 5. 保存为 PNG
    HRESULT hr = image.Save(filePath, Gdiplus::ImageFormatPNG);

    // 6. 清理（注意：image 已接管 hBitmap，所以不要 delete bitmap）
    tempDC.SelectObject(pOldBitmap);
    // bitmap 析构时不会释放 hBitmap，因为已被 image.Attach() 接管
    // 所以我们让 image 负责释放

    return SUCCEEDED(hr);
}
```