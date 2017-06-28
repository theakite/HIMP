<!DOCTYPE>
<html>
    
    <head>
    </head>

    <body>
        <?php
            $filename = 'material.txt';
            $testContent = htmlspecialchars($_POST['text']);

            if (file_exists($filename)) {
                echo "File Exists";
            }
            else {
                echo "File Doesn't Exist";
            }

            if (is_readable($filename)) {
                echo "File is Readable";
            }
            else {
                echo "File is NOT Readable";
            }

            if (is_writable($filename)) {
                if (!$handle = fopen($filename, 'a')) {
                    echo "Cannot open file ($filename)";
                    exit;
                }

                if (fwrite($handle, $testContent) === FALSE) {
                    echo "Cannot write to file";
                    exit;
                }

                echo "Success";

                fclose($handle);
            }

            else {
                echo "Didn't work";
            }
        ?>
    </body>

</html>