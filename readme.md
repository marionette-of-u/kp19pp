# �T�v
LALR(1) �p�[�T�W�F�l���[�^�ł��B  
�p�����v���W�F�N�g caper�i<http://caper.googlecode.com/svn/trunk/caper/site/caper.html>�j

# ����

## �I�[�L����`��

    <token_desc> token{
        <right>{ unary_minus; }

        <left>{
            ast;
            pls, mns;
        }

        l_paren, r_paren;
        id<int>;
    }
��s�����Ă����܂��傤�B  
`<token_desc> token`  
�����ł́u�I�[�L���̃g�[�N���̐錾���A�D�揇�ʂɊւ��č~���Ɂi`<token_desc>`�j�v�u�g�[�N����enum�y��prefix��`token`�v�Ɏw�肵�Ă��܂��B  
���̐ݒ�̗]�n�Ƃ��āA`<token_asc>`�i�D�揇�ʂ������j�Atoken����`hogehoge`��`piyopiyo`�Ȃǂ̍D���ȕ�����ɂł��܂��B  
`<right>{ unary_minus; }`  
�����ł́A�����ʓ����Ɍ����I�[�L���̌����������E�ɐݒ肵�Ă��܂��B�܂��A�����`unary_minus`�Ƃ����g�[�N���́A���ꎩ�̂����ڍ\���̒��Ŏg����L���łȂ��Ă��\���܂���B���ہA����`unary_minus`�͍\����`���ŗD�揇�ʂ�ύX���邽�߂����Ɏg�p����܂��B
`<left>{`  
�����`<right>`�Ɠ��l�A�����ʓ��Ɍ����I�[�L�����������Ɏw�肷�邽�߂ɋL�q���Ă��܂��B  
`ast;`  
`pls, mns;`  
���̎O�̏I�[�L���ɂ͂��ꂼ��A�s���ɃZ�~�R�������L�q����Ă��܂��B�I�[�L�����Z�~�R�����ŋ�؂�ƁA�D�揇�ʂ�ύX���鎖���ł��܂��B  
����ł�`<token_desc>`�ƍ~���ɂȂ����Ă���̂ŗD�揇�ʂ�`ast > pls = mns`�ɂȂ�܂��B  
`l_paren, r_paren;`�͂��ꂼ��A���ۊ��ʁA�E�ۊ��ʂł��B  
`id<int>;`��`int`�^�̒l�����I�[�L���ł��B�I�[�L�����l�������Ă���ƁA�Z�}���e�B�b�N�A�N�V�����Ɉ����Ƃ��Ďg���܂��B

## ���@��`��

    <grammar> grammar{
        E<int>
            : [make_pls] E(0) pls E(1)
            | [make_mns] E(0) mns E(1)
            | [make_mlt] E(1) ast E(0)
            | [make_exp] l_paren E(0) r_paren
            | [make_ums] <uminus> mns E(0)
            | [make_id]  id(0)
            ;
    }
`<grammar> grammar`�ɂ����āA�R���ʂň͂܂ꂽ�����́u���@���`����v�Ƃ����w�b�_�̋L�q�ł���K�{�ł��B  
���̎���`grammar`���A�o�͂��ꂽ���Ƀp�[�T�[���͂����O��ԂƂȂ�܂��B  
`E<int>`�����Ŕ�I�[�L��E�̒�`���s���܂��B��I�[�L���ɂ͒l��\���^���K�{�ɂȂ��Ă��܂��B�����ł�int�^��z�肵�Ă���̂�`<int>`�ƂȂ��Ă��܂��B  
`[make_pls] E(0) pls E(1)`�u���P�b�g�ň͂܂ꂽ`make_pls`�̓Z�}���e�B�b�N�A�N�V�����ł��B�Ȃ��ꍇ�A����`[]`���Ɓu�Z�}���e�B�b�N�A�N�V���������v�̕��@��`�ɂȂ�܂��B  
�����Ď��̕��@��`�Ɍ����`E(0) pls E(1)`�́u�I�[�L��`pls`�����񂾍�����`E`��1�Ԗڂ̈����ɂƉE����`E`��2�Ԗڂ̈����Ɏ��v�Ƃ��������Ӗ����܂��B