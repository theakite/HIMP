<!DOCTYPE>
<html>
    <head></head>

    <body>
        <?php 

            if(file_exists('parameter.xml')) {
                $xml = simplexml_load_file('parameter.xml');
                foreach ($xml->children() as $child) {
                    foreach ($child->attributes() as $type => $data) {
                        if ($type == "display" && $data != "false") {
                            echo $data;
                            echo " ";
                        }
                    }
                }
            }

            else {
                echo "Didn't work";
                //exit('Failed to open file');
            }

        ?>
    </body>
</html>