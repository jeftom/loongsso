<?php
session_start();

include_once "page.inc.php";
$pp    = new pp;
$size  = 6;

$rs    = new SQLite3("guestbook.sdb");

//id, uid, username, reg_time, post_time, ip, ip_pos, content
if(is_array($_SESSION["user"]) && $_POST["book"])
{
	$userip  = ip();
	$content = nl2br(stripslashes(Replace($_POST["content"])));
	
	$query   = "INSERT INTO guestbook (uid, username, reg_time, post_time, ip, content) VALUES ('{$_SESSION["user"]["id"]}', '{$_SESSION["user"]["username"]}', '{$_SESSION["user"]["time"]}', '".time()."', '{$userip}', '{$content}')";
	
	$rs->query($query);
	$rs->close();
	
	echo "<script>alert('����ɹ�');window.location.href = 'index.php';</script>";
	exit();
}

$co   = $rs->fetch_line("SELECT COUNT(*) AS c FROM guestbook");
$page = $pp->show($co["c"], $size);
$list = $rs->fetch_all("SELECT id, uid, username, reg_time, post_time, ip, content FROM guestbook ORDER BY id DESC LIMIT {$pp->limit}");

$rs->close();

function Replace($str)
{
	if(is_null($str)) return $str;

	$word = array("<" => "&lt;", ">" => "&gt;", "'" => "&quot;", '"' => "&quot;");
	return strtr($str, $word);
}

//��ȡ�ͻ��˵�IP
function ip()
{
	if(getenv('HTTP_CLIENT_IP') && strcasecmp(getenv('HTTP_CLIENT_IP'), 'unknown')) 
	{
		$onlineip = getenv('HTTP_CLIENT_IP');
	}
	elseif(getenv('HTTP_X_FORWARDED_FOR') && strcasecmp(getenv('HTTP_X_FORWARDED_FOR'), 'unknown'))
	{
		$onlineip = getenv('HTTP_X_FORWARDED_FOR');
	}
	elseif(getenv('REMOTE_ADDR') && strcasecmp(getenv('REMOTE_ADDR'), 'unknown')) 
	{
		$onlineip = getenv('REMOTE_ADDR');
	} 
	elseif(isset($_SERVER['REMOTE_ADDR']) && $_SERVER['REMOTE_ADDR'] && strcasecmp($_SERVER['REMOTE_ADDR'], 'unknown')) 
	{
		$onlineip = $_SERVER['REMOTE_ADDR'];
	}
	$onlineip = preg_replace("/^([\d\.]+).*/", "\\1", $onlineip);
	return $onlineip;
}

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"  xml:lang="zh-CN" lang="zh-CN">
<head>
<title>���Ա� ��ҳ</title>
<meta http-equiv="Content-Type" content="text/html; charset=gbk" />
<link href="css/global.css" rel="stylesheet" type="text/css" />
  <script language="JavaScript">
  <!--
	
	var eInterval;
	var loong_status;
	var intervalCount;
	var maxIntervalCount;
	
	eInterval        = false;
	loong_status     = -1;
	intervalCount    = 0;
	maxIntervalCount = 100;
	
	var http_response_t = {
		HTTP_RESPONSE_EMAIL_NO            : 0,
		HTTP_RESPONSE_EMAIL_OK            : 1,
		HTTP_RESPONSE_LOGIN_OK            : 2,
		HTTP_RESPONSE_LOGOUT_OK           : 3,
		HTTP_RESPONSE_REGISTER_OK         : 4,
		HTTP_RESPONSE_VALIDATE_NO         : 5,
		HTTP_RESPONSE_VALIDATE_OK         : 6,
		HTTP_RESPONSE_USERNAME_NO         : 7,
		HTTP_RESPONSE_USERNAME_OK         : 8,
		HTTP_RESPONSE_PASSWORD_NO         : 9,
		HTTP_RESPONSE_EMAIL_EXISTS        : 10,
		HTTP_RESPONSE_UNKNOWN_TYPE        : 11,
		HTTP_RESPONSE_UNKNOWN_MODULE      : 12,
		HTTP_RESPONSE_USERNAME_EXISTS     : 13,
		HTTP_RESPONSE_EMAIL_NOT_EXISTS    : 14,
		HTTP_RESPONSE_USERNAME_NOT_EXISTS : 15
	};

	
	function is_password(str)
	{
		var i, c;
		if(str.length < 5 || str.length > 19)
		{
			return false;
		}
		for(i=0; i < str.length; i++)
		{
			c = str.charAt(i);
			if( (c < "0" || c > "9") && (c < "a" || c > "z" ) && (c < "A" || c > "Z" ))
			{
				return false;
			}
		}
		return true;
	}


	function isEmail(strEmail) 
	{
		if (strEmail.search(/^\w+((-\w+)|(\.\w+))*\@[A-Za-z0-9]+((\.|-)[A-Za-z0-9]+)*\.[A-Za-z0-9]+$/) != -1)
		{
			return true;
		}

		return false;
	}

	function is_username(username)
	{
		var patrn = /^[a-zA-Z0-9\u4e00-\u9fa5]{2,20}$/; 


		if(!patrn.exec(username))
		{
			return false;
		}

		return true;
	}

	function loginIntervalProc()
	{
		var frm;

        if (loong_status == -1 && intervalCount < maxIntervalCount)
        {
            intervalCount++;
            return ;
        }
		
		switch (loong_status)
		{
			case http_response_t.HTTP_RESPONSE_LOGOUT_OK :
				alert("�˳��ɹ�");
				window.location.href = "http://www.xxxabc.cn:8080/index.php";
				break;
			default:
				alert("��֤��ʱ");
		}
		

		clearInterval(eInterval);
		intervalCount = 0;
        eInterval     = false;
		login_status  = -1;
	}

	function load_script(url)
	{
		var newScript  = document.createElement("script");
		newScript.type = "text/javascript";
		
		if(url.indexOf("?") == -1)
		{
        	newScript.src  = url + "?rand=" + Math.random();
		}
		else
		{
			newScript.src  = url + "&rand=" + Math.random();
		}
		document.getElementsByTagName("head")[0].appendChild(newScript);
	}


	function disposal()
	{
		var url;
		
		url = "http://www.cellphp.com:7171/?module=logout";
		load_script(url);

	    eInterval = setInterval(function()
        {
            loginIntervalProc();
        }
        , 100);

		return false;
	}
  //-->
  </script>
</head>
<body>

<!-- ͷ�� -->
<div class="top">
	<div class="l">
		<img src="images/logo.gif" alt="" />
	</div>
	<div class="top_ad r">

	</div>
</div>

<!-- ���� -->
<div class="menu">
	<span class="l">Ŀǰ���� <span class="fb fc5"><?=$co["c"];?></span> �����ԣ�&nbsp;&nbsp;<?=$page;?></span>
	<span class="r">
		<? if(!is_array($_SESSION["user"])): ?>
		<a href="login.html">��¼</a>
		<a href="reg.html">ע��</a>
		<? else: ?>
		�û���: <?=$_SESSION["user"]["username"];?>&nbsp;&nbsp;&nbsp;ע��ʱ��: <?=date("Y-m-d H:i:s", $_SESSION["user"]["time"]);?>&nbsp;&nbsp;&nbsp;<a href="javascript:void(0);" onclick="return disposal();">�˳�</a>&nbsp;&nbsp;
		<? endif; ?>		
	</span>
</div>
<div class="bottombg"></div>

<!-- ѭ���������� -->
<? foreach($list as $item): ?>
<div class="book">
	<div class="book_L l">
		<div class="book_L_top">��<span class="fb"><?=$item["id"]?></span>��</div>
		<div class="book_L_tx"><img src="nice.gif" width="82" height="81" /></div>
		<h1><?=$item["username"]?></h1>
		<div class="book_L_p">
			<p>IP: <?=$item["ip"]?></p>
			<p>Reg: <?=date("Y-m-d", $item["reg_time"]);?></p>
			<p><?=date("Y-m-d H:i:s", $item["post_time"]);?></p>
		</div>
	</div>

	<div class="book_R f14px r">
		<?=$item["content"]?>
	</div>
</div>
<div class="bottombg"></div>

<? endforeach; ?>
<!-- ѭ���������� END -->

<!-- �������� -->
<? if(is_array($_SESSION["user"])): ?>
<div class="publish">
	<h1>���ٷ�������</h1>
	<div class="publish_L fb l">���ݣ�</div>

	<form id="form1" name="form1" method="post" action="index.php">

		<textarea name="content" class="publish_L_p l" id="content"></textarea>
		<div class="submit">
		  <label><input class="publish_submit" type="submit" name="book" value="�ύ" /></label>
		  <label><input class="publish_submit" type="reset" name="Submit" value="����" /></label>
		</div>

	</form>

</div>
<? else: ?>
<div class="publish">
	<h1>����<a href="login.html">��¼</a>��<a href="reg.html">ע��</a> �ٽ�������</h1>
</div>
<? endif; ?>
<div class="footer">
	<p>��Ȩ���У�loongSSO</p>
	<p>����֧�֣���ҹ(�����) lijinxing@gmail.com </p>
</div>



</body>
</html>