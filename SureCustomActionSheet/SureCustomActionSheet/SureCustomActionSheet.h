//
//  SureCustomActionSheet.h
//  SureCustomActionSheet
//
//  Created by 刘硕 on 2017/5/5.
//  Copyright © 2017年 刘硕. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SureCustomActionSheet : UIView
- (instancetype)initWithTitleView:(UIView*)titleView
                       optionsArr:(NSArray*)optionsArr
                      cancelTitle:(NSString*)cancelTitle
                    selectedBlock:(void(^)(NSInteger))selectedBlock
                      cancelBlock:(void(^)())cancelBlock;

@end
