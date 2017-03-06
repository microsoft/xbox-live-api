var ieBrowser = true;
var otherBrowser = false;
window.onload = ResizeWindow;
window.onresize=ResizeWindow;
function SetBrowserID()
{
    var browser = navigator.appName;
    if(browser == "Microsoft Internet Explorer")
    {
        ieBrowser = true;
        otherBrowser = false;
    }
    else
    {
        ieBrowser = false;
        otherBrowser = true;
    }
}

function saveAll()
{
}

function AncestorTop(element)
{
    if(element != null)
    {
        var top;
        if(typeof element.offsetTop !== "undefined")
        {
            top = element.offsetTop + AncestorTop(element.offsetParent);
        } else
        {
            top = element.y + AncestorTop(element.offsetParent);
        }
        return top;
    } else
    {
        return 0;
    }
}

function AncestorLeft(element)
{
    if(element != null)
    {
        var left;
        if(typeof element.offsetLeft !== "undefined")
        {
            left = element.offsetLeft + AncestorLeft(element.offsetParent);
        } else
        {
            left = element.x + AncestorLeft(element.offsetParent);
        }
        return left;
    } else
    {
        return 0;
    }
}

function GetElementTop(element)
{
    var top = element.offsetTop + AncestorTop(element.offsetParent);

    if(typeof element.offsetTop !== "undefined")
    {
        top = element.offsetTop + AncestorTop(element.offsetParent);
    } else
    {
        top = element.y + AncestorTop(element.offsetParent);
    }
    return top;
}

function GetElementLeft(element)
{
    var left;

    if(typeof element.offsetLeft !== "undefined")
    {
        left = element.offsetLeft + AncestorLeft(element.offsetParent);
    } else
    {
        left = element.x + AncestorLeft(element.offsetParent);
    }
    return left;
}

function GetElementHeight(element)
{
    if (typeof element.clip !== "undefined")
    {
        return element.clip.height;
    } else
    {
        if (typeof element.offsetHeight !== "undefined")
        {
            return element.offsetHeight;
        } else
        {
            return element.style.pixelHeight;
        }
    }
}

function GetElementWidth(element)
{
    if (typeof element.clip !== "undefined")
    {
        return element.clip.width;
    } else
    {
        if (element.offsetWidth !== "undefined")
        {
            return element.offsetWidth;
        } else
        {
            return element.style.pixelWidth;
        }
    }
}

function GetWindowHeight()
{
    var height;
    if (window.innerHeight)
    {
        height = window.innerHeight;
    } else
    {
        if (document.body.offsetHeight)
        {
            height = document.body.offsetHeight;
        }
    }
    return height;
}

function GetWindowWidth()
{
    var width;
    if (window.innerWidth)
    {
        width = window.innerWidth;
    } else
    {
        if(document.body.offsetWidth)
        {
            width = document.body.offsetWidth;
        }
    }
    return width;
}

function SetX(element, x)
{
    element.style.left = x;
}

function SetY(element, y)
{
    element.style.top = y;
}

function SetHeight(element, height)
{
    element.style.height = height;
}

function SetWidth(element, width)
{
    element.style.width = width;
}

// returns the FIRST child of element that has a class attribute that matches class
function GetChildElementByClass(element, elementClass)
{
    var retElement = null;
    for( var child = element.firstChild; child != null; child = child.nextSibling)
    {
        if(child && child.nodeType != 1)
        {
            var childClass = child.getAttribute("class");
            if( typeof childClass !== "undefined" && childClass == elementClass)
            {
                retElement = child;
                return retElement;
            }
        }
    }
    return retElement;
}

function loadAll()
{
//debugger;
    SetBrowserID();
    var heightDiff = 1;
    setWindowSize(heightDiff);

    // select first language as default
    var languages = document.getElementsByClassName("languageSpecificSelector");
    if (languages[0]) {
        var children = languages[0].children;
        if (children[0]) {
            selectLanguage(children[0].className);
        }
    }
}

function ResizeWindow() {
    var heightDiff = 10
    setWindowSize(heightDiff);

}
function setWindowSize(heightDiff) {

    if (document.body.clientWidth == 0) return;

    var height;
    var headerHeight;
    var header;
    var mainSection;

    height = GetWindowHeight();

    mainSection = document.getElementById("mainSection");
    if (mainSection == null) return;

    header = document.getElementById("header");
    headerHeight = GetElementHeight(header) + 5;

    if (document.body.offsetHeight > header.offsetHeight + 10)
        SetHeight(mainSection, height - headerHeight);
    else
        mainSection.style.height = 0;
    SetX(mainSection, 0);
    SetY(mainSection, headerHeight - heightDiff);

    try {
        mainSection.setActive();
    }
    catch (e) {
    }
}

function CopyCode(key) {

    var trElements = document.all.tags("pre");

    var i;

    for (i = 0; i < trElements.length; ++i) {
        if (key.parentElement.parentElement.parentElement == trElements[i].parentElement)
        {
            window.clipboardData.setData("Text", trElements[i].innerText);
        }
    }
}
function ExpandCollapse(siblingElement)
{
    var state = siblingElement.getAttribute("state");
    var nextSibling = siblingElement.nextSibling;
    var expand = document.getElementById("expandImage").getAttribute("src");
    var collapse = document.getElementById("collapseImage").getAttribute("src");

    if(state == "expanded")
    {
        siblingElement.setAttribute("state", "collapsed");
        siblingElement.style.backgroundImage = "url(" + expand + ")";
        nextSibling.style.display = "none";
    } else
    {
        siblingElement.setAttribute("state", "expanded");
        siblingElement.style.backgroundImage = "url(" + collapse + ")";
        nextSibling.style.display = "inline";
    }
}

function InElementArea(element, e)
{
    var left = GetElementLeft(element);
    var top = GetElementTop(element);
    var width = GetElementWidth(element);
    var height = GetElementHeight(element);


    var posX;
    var posY;

    if (!e) var e = window.event;

    if (e.pageX || e.pageY)
    {
        posX = e.pageX;
        posY = e.pageY;
    } else if (e.clientX || e.clientY)
    {
        posX = e.clientX + document.body.scrollLeft
            + document.documentElement.scrollLeft;;
        posY = e.clientY + document.body.scrollTop
            + document.documentElement.scrollTop;
    }
    if(posX >= left && posX <= left + width && posY >= top && posY <= top + height)
    {
        return true;
    }
    return false;
}

function selectLanguage(lang) {
    var elems = document.getElementsByTagName("SPAN");
    for (var i in elems) {
        var elem = elems[i];
        var p = elem.parentElement;
        if (!p) continue;
        if (p.tagName != "SPAN") continue;
        if (p.className == "languageSpecificText") {
            if (elem.className == lang) {
                elem.style.display = "";
            } else {
                elem.style.display = "none";
            }
        } else if (p.className == "languageSpecificSelector") {
            if (elem.className == lang) {
                elem.style.color = "orangered";
            } else {
                elem.style.color = "gray";
            }
        }
    }
}

// getElementsByClassName is supported since IE9 of EcmaScript 5
// while XMetal only supports EcmaScript 3 now, use this one instead
if (!document.getElementsByClassName) {
    document.getElementsByClassName = function getElementsByClassName(classname) {
        var a = [];
        var re = new RegExp('(^| )' + classname + '( |$)');
        var els = document.getElementsByTagName("*");
        for (var i = 0, j = els.length; i < j; i++)
            if (re.test(els[i].className)) a.push(els[i]);
        return a;
    }
}

