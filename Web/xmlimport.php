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

                //removeWhitespace($topNode);
                assembleChildren($topNode);

                //$doc->saveXML();
                echo "End of File";
            }

            else {
                print "<p>Unable to find XML Document</p>";
            }


// End of file, begin function declarations



            function assembleChildren($node) {
                //removeWhitespace($node);//gets rid of extra nodes that come from the formatting whitespace
                removeComments($node);
                if ((int)$node->nodeType == 1 && $node->hasChildNodes() == true) {
                    //removeWhitespace($node);
                    //print '<li>'; //if there are children, there will list items;
                    foreach($node->childNodes as $childNode) {
                        if (((int)($childNode->nodeType == 1)) && ($childNode->hasAttribute("display") == true)) {
                            $displayAtrb = $childNode->getAttribute('display');
                            if ($displayAtrb != '__false') {//TODO: this should move to config file
                                print '<li>';
                                if (!$childNode->hasChildNodes()) {
                                    print '<a href = "./ruledisplay.php?room=';
                                    print $childNode->getAttribute('room');
                                    print '&hotCount=';
                                    print $childNode->getAttribute('hotCount');
                                    print '&coldCount=';
                                    print $childNode->getAttribute('coldCount');
                                    print '">';
                                }
                                print $displayAtrb; //we want to display it
                                if ($childNode->hasChildNodes()) {
                                    print '<ul>';
                                    assembleChildren($childNode);
                                    print "</ul>";
                                }
                                else {
                                    print "</a>";
                                    if ($node->getAttribute('class') == "__editable") {
                                        print '<li><span contenteditable = "true">';
                                    }
                                    print $node->textContent;
                                    //print $childNode->nodeType;
                                    if ($node->getAttribute('class') == "__editable") {
                                        print '</span></li>';
                                    }
                                }
                            }
                        }

                        /*else {
                            if(($childNode->nodeType == 3)) {
                                if ($node->getAttribute('class') == "__editable") {
                                    print '<li><span contenteditable = "true">';
                                }
                                print $node->textContent;
                                print $childNode->nodeType;
                                if ($node->getAttribute('class') == "__editable") {
                                   print '</span></li>';
                                }
                            }
                        }*/

                        //removeWhitespace($childNode);

                        /*if(($childNode->hasChildNodes()) && ($childNode->getAttribute('display') != '__false')) {//if its children have children, call again
                            //removeWhitespace($childNode);
                            assembleChildren($childNode);
                            if (($childNode->hasChildNodes()) && ($node->nodeType == 1)) {
                                print '</ul>';
                            }
                        }

                        else {//children don't have children
                            //print "<div></div>";
                        }*/
                    }
                    //print '</ul>';

                }
                return;
            }

            function removeWhitespace(&$node) {
                if ($node->hasChildNodes()) {
                    $size = $node->childNodes->length;
                    for ($i = 0; $i < $size; $i++) {
                        if(($node->childNodes[$i]->nodeType == 3) || ($node->childNodes[$i]->nodeType == 8)) {
                            $node->removeChild($node->childNodes[$i]);
                        }
                    }
                }
                return;
            }

            function removeComments(&$node) {
                if ($node->hasChildNodes()) {
                    $size = $node->childNodes->length;
                    for ($i = 0; $i < $size; $i++) {
                        if($node->childNodes[$i]->nodeType == 8) {
                            $node->removeChild($node->childNodes[$i]);
                        }
                    }
                }
                return;
            }

        ?>
    </body>
</html>