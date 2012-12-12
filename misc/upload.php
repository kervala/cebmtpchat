<?

ini_set("display_errors","1");
error_reporting(E_ALL);

// configuration
$enable_debug = false;
$files_folder = $_SERVER['DOCUMENT_ROOT']."/files";
$logs_folder = $_SERVER['DOCUMENT_ROOT']."/logs";

$ip = getenv('REMOTE_ADDR');
$user_agent = getenv('HTTP_USER_AGENT');

// supports POST, PUT and HEAD

if ($enable_debug)
{
	$log = fopen("$logs_folder/upload.txt", "a");

	// log all variables
	$headers = $_SERVER;

	foreach ($headers as $header => $value)
	{
		fwrite($log, "$header: $value\n");
	}

	$headers = $_POST;

	foreach ($headers as $header => $value)
	{
		fwrite($log, "POST: $header: $value\n");
	}

	$headers = $_GET;

	foreach ($headers as $header => $value)
	{
		fwrite($log, "GET: $header: $value\n");
	}
}

if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_FILES))
{
	foreach($_FILES as $file)
	{
		$filename = basename($file['name']);

		if ($enable_debug) fwrite($log, "filename = $filename\n");

		$res = move_uploaded_file($file['tmp_name'], "$files_folder/$filename");
	}
}
else if ($_SERVER['REQUEST_METHOD'] == 'PUT')
{
	$size = isset($_SERVER['CONTENT_LENGTH']) ? $_SERVER['CONTENT_LENGTH']:0;
	$path = isset($_SERVER['PATH_TRANSLATED']) ? $_SERVER['PATH_TRANSLATED']:"";

	if ($path)
	{
		$filename = basename($path);
	}
	else if (isset($_GET['filename']))
	{
		$filename = basename($_GET['filename']);
		$path = "$files_folder/$filename";
	}

	if ($enable_debug) fwrite($log, "filename = $filename\n");
	
	if ($path != "$files_folder/$filename" || file_exists($path))
	{
		header('HTTP/1.1 403 Forbidden');
	}
	else
	{
		// read the whole stream
		$putdata = fopen("php://input", "r");
		$content = "";
		while ($pdata = fread($putdata, 1024)) $content .= $pdata;
		fclose($putdata);

		// write stream to file
		if ($size > 0 && $content && file_put_contents($path, $content, LOCK_EX) == $size)
		{
			header('HTTP/1.1 201 Created');
		}
		else
		{
			header('HTTP/1.1 204 No Content');
		}
	}
}
else if ($_SERVER['REQUEST_METHOD'] == 'HEAD' || $_SERVER['REQUEST_METHOD'] == 'GET')
{
	$filename = "";

	if (isset($_GET['filename']))
	{
		$filename = basename($_GET['filename']);
	}
	
	if ($filename && file_exists("$files_folder/$filename"))
	{
		header('HTTP/1.1 200 OK');

		$size = filesize("$files_folder/$filename");
		$time = date("r", filemtime("$files_folder/$filename"));
		
		// we should encode MD5 in Base64 but Android 1.6 doesn't support Base64 decoding
		$md5 = md5_file("$files_folder/$filename");

		header('Accept-Ranges: bytes');
		header("Content-Length: $size");
		header("Last-Modified: $time");
		header('Content-Type: image/jpeg');
		header("Content-MD5: $md5");
		
		if ($_SERVER['REQUEST_METHOD'] == 'GET')
		{
			header("Content-Disposition: inline; filename=\"$filename\"");
			print file_get_contents("$files_folder/$filename");
		}
	}
	else
	{
		header('HTTP/1.1 404 Not Found');
	}
}
else if ($_SERVER['REQUEST_METHOD'] == 'DELETE')
{
	// 200 (OK)
	// 202 (Accepted)
	// 204 (No Content)
}

if ($enable_debug) fclose($log);

?>
