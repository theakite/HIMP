<!DOCTYPE>
<html>
    <head></head>

    <body>
        <?php 

            if(file_exists('parameter.xml')) {
                $doc = new DOMDocument();
                $doc->load(parameter.xml);
                $listONodes = new DOMNodeList;
                $listONodes=$doc->childNodes;
                foreach ($listONodes->item as $element) {
                    print "<p> $element->textContent </p>";
                }
                $doc->saveXML();
                echo "Here";
            }

            else {
                echo "Didn't work";
                //exit('Failed to open file');
            }

        ?>
    </body>
</html>