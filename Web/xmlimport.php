<!DOCTYPE html>
<html>
    <head>
        <title>HIMP Editor</title>
    </head>

    <body>
    <h1>HIMP Editior</h1>
    <h2>Properties</h2>
        <?php 

            if(file_exists('./parameter.xml')) {
                $doc = new DOMDocument();
                $doc->load('parameter.xml');

                $topNode = new DOMNode;
                $topNode = $doc->childNodes->item(0); //assumes that we only care about the first child (there shouldn't ever be any other top-level children)

                removeWhitespace($topNode);
                assembleChildren($topNode);

                //$doc->saveXML();
                echo "End of File";
            }

            else {
                print "<p>Unable to find XML Document</p>";
            }

            function assembleChildren($node) {
                //removeWhitespace($node);//gets rid of extra nodes that come from the formatting whitespace
                if ($node->hasChildNodes() && $node->nodeType == 1) {
                    removeWhitespace($node);
                    print '<ul>'; //if there are children, there will list items
                    foreach($node->childNodes as $childNode) {
                        $displayAtrb = $childNode->getAttribute('display');
                        if ($displayAtrb != '__false') {
                            print '<li>';
                            print $displayAtrb; //we want to display it
                        }

                        //removeWhitespace($childNode);

                        if($childNode->hasChildNodes()) {//if its children have children, call again
                            //removeWhitespace($childNode);
                            assembleChildren($childNode);
                        }

                        else {//children don't have children
                            echo '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000';
                            print '</li>';
                        }
                    }
                    print '</ul>';

                }
                else if($node->nodeType == 1) { //node has no children
                    if($node->getAttribute('display') != '__false') {
                        print '<span contenteditable = "true">';
                        print $node->textContent;
                        print '</span></li>';
                    }
                }
                return;
            }

            function removeWhitespace(&$node) {
                $size = $node->childNodes->length;
                for ($i = 0; $i < $size; $i++) {
                    if($node->childNodes[$i]->nodeType == 3 || $node->childNodes[$i]->nodeType == 8) {
                        $node->removeChild($node->childNodes[$i]);
                    }
                }
            }

        ?>
    </body>
</html>