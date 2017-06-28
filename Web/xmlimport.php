<!DOCTYPE>
<html>
    <head></head>

    <body>
        <?php 

            if(file_exists('parameter.xml')) {
                echo "we got here 1";
                $xml = simplexml_load_file('parameter.xml');
                echo "we got here 2";
                echo $xml->fileUpdate;
            }

            else {
                echo "Didn't work";
                //exit('Failed to open file');
            }

        ?>
    </body>
</html>