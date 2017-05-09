# SureCustomActionSheet
######前言
本文为iOS自定义视图封装《一劳永逸》系列的第四期，旨在提供封装思路，结果固然重要，但理解过程才最好。授人以鱼不如授人以渔。️文章旨在帮助封装程度较低的朋友们，大神可无视勿喷。
######历史文章链接列表：
- [一劳永逸，iOS引导蒙版封装流程](http://www.jianshu.com/p/dfc3ecdd5810)
- [一劳永逸，iOS网页视图控制器封装流程](http://www.jianshu.com/p/553424763585)
- [一劳永逸，iOS多选弹窗封装流程](http://www.jianshu.com/p/99a33ada38a6)

######正文
最近更新项目需求，需要重构导航选取模块，故将封装流程进行分享，效果图如下：

![导航选取弹窗](http://upload-images.jianshu.io/upload_images/1767950-5fd3bd47558598b2.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

根据效果图情况，可知标题栏位置需自定义，且选项位置等文字样式可调节，因此无法利用系统的**UIActionSheet**或**UIAlertController**进行实现，需自定义视图，并考虑到适用场景，该**ActionSheet**后续会用于其他功能模块中，所以要封装成通用类。

继续分析需求，用什么控件作为主体会更好呢？没错，**UITableView**是在适合不过了，见刨析图：
![刨析图](http://upload-images.jianshu.io/upload_images/1767950-d979332cd63964be.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

如上设计的优势在于可将自定义**View**传入做为**TableView**的表头视图**tableHeaderView**。对于选项位置若需定制其它样式可自定义**Cell**进行设置。因此可满足自定义**ActionSheet**的多种场景。

根据刨析图，首先我们分别创建**maskView**与主体**TableView**。这里需注意的是为了实现效果我们需要将**TableView**的颜色置为透明。
```
_tableView.backgroundColor = [UIColor clearColor];
```
并且我们要将**TableView**分为两组，即主体选项与取消按钮分离，因此设置代理方法：
```
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return (section == 0)?_optionsArr.count:1;
}
```

接下来进行处理视图的圆角效果。TableView主体做圆角效果这个不用多说，需要注意的是如何处理好**TableViewCell**的圆角形式，若仍然简单设置**layer.cornerRadius**会出现如下情况：
![效果图](http://upload-images.jianshu.io/upload_images/1767950-ba9a7a8d22ba24c7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

显然很难看，设计看到估计会崩溃，因此我们需要做的处理为仅为选项中的最后一项做圆角处理，即图中的高德地图选项，并且为了美观效果，要达到只为其左下角与右下角做处理。

这里我们需要借助**UIBezierPath**与**CAShapeLayer**进行实现。判断是否为最后选项，然后进行如下设置。
```
UIBezierPath *maskPath = [UIBezierPath bezierPathWithRoundedRect:
                                      cell.contentView.bounds byRoundingCorners:
                                      UIRectCornerBottomLeft|UIRectCornerBottomRight cornerRadii:
                                      CGSizeMake(10, 10)];
CAShapeLayer *maskLayer = [[CAShapeLayer alloc]init];
maskLayer.frame = cell.contentView.bounds;
maskLayer.path = maskPath.CGPath;
cell.layer.mask = maskLayer;
```
其中**byRoundingCorners**即为设置所需处理边角参数。有如下枚举克进行选择：
```
typedef NS_OPTIONS(NSUInteger, UIRectCorner) {
    UIRectCornerTopLeft     = 1 << 0,
    UIRectCornerTopRight    = 1 << 1,
    UIRectCornerBottomLeft  = 1 << 2,
    UIRectCornerBottomRight = 1 << 3,
    UIRectCornerAllCorners  = ~0UL
};
```
比如将**byRoundingCorners**设置为**UIRectCornerTopLeft| UIRectCornerBottomRight**，即左上与右下设置，**View**的效果为：
![效果图](http://upload-images.jianshu.io/upload_images/1767950-98cc884c7241ff33.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

经过上述调整，视图的圆角效果完成，最后设置组尾透明视图即可：
```
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return SPACE;
}

- (UIView*)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section {
    UIView *footerView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, tableView.bounds.size.width, SPACE)];
    footerView.backgroundColor = [UIColor clearColor];
    return footerView;
}
```
做好如上处理后基本的UI效果即已完成。

接下来我们考虑外漏方法问题，简单模拟**UIActionSheet**的创建形式，公开方法：
```
- (instancetype)initWithTitleView:(UIView*)titleView
                       optionsArr:(NSArray*)optionsArr
                      cancelTitle:(NSString*)cancelTitle
                    selectedBlock:(void(^)(NSInteger))selectedBlock
                      cancelBlock:(void(^)())cancelBlock;
```
调用如下：
```
SureCustomActionSheet *optionsView = [[SureCustomActionSheet alloc]initWithTitleView:self.headView optionsArr:self.dataArr cancelTitle:@"取消" selectedBlock:^(NSInteger index) {
        
} cancelBlock:^{
        
}];
[self.view addSubview:optionsView];
```
这样即可将所需头视图、取消文字传入，并处理选项事件等。

最后简单给予视图显示与隐藏的效果，并在欠当的时机调用即可，且这里我们需要调节**TableView**的高度，使其适应所包含内容高度。
```
- (void)show {
    _tableView.frame = CGRectMake(SPACE, Screen_height, Screen_Width - (SPACE * 2), _tableView.rowHeight * (_optionsArr.count + 1) + _headView.bounds.size.height + (SPACE * 2));
    [UIView animateWithDuration:.5 animations:^{
        CGRect rect = _tableView.frame;
        rect.origin.y -= _tableView.bounds.size.height;
        _tableView.frame = rect;
    }];
}

- (void)dismiss {
    [UIView animateWithDuration:.5 animations:^{
        CGRect rect = _tableView.frame;
        rect.origin.y += _tableView.bounds.size.height;
        _tableView.frame = rect;
    } completion:^(BOOL finished) {
        [self removeFromSuperview];
    }];
}
```
```
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [self dismiss];
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self dismiss];
}
```
至此，该需求效果已基本完成，如上摘取部分代码，demo已上传github，需要可自行下载。
[一劳永逸，iOS自定义ActionSheet封装流程demo](https://github.com/LSure/SureCustomActionSheet)

暂时写到这里，喜欢的可以点个赞或关注我，比心(｡･ω･｡)ﾉ
