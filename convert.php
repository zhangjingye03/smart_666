<?php
//header("Content-Type: text/html; charset=gb2312");
  if (@$_POST['flag'] && @$_FILES["file"]["error"] <= 0) {
    $p = file_get_contents($_FILES['file']['tmp_name']);//$_POST['str'];
  //  $p = file_get_contents("test.csv");

    $qt = explode("\r\n", $p);
    $q = array(); $qi = 0;

    // read valid values
    for ($i = 1; $i < sizeof($qt) - 1; $i++) {
      $qq = explode("`", $qt[$i]);
      if (sizeof($qq) < 12) continue;
      if ($qq[7] == 0 || sizeof($qq[6]) != 1) continue;
      $q[$qi++] = $qt[$i];
    }

    // randomize
    $r = $q; $usedIndex = array(); $randNum = 0;
    for ($i = 0; $i < sizeof($r); $i++) {
      while (true) {
        $randNum = rand(0, sizeof($r) - 1);
        $conflict = false;
        for ($j = 0; $j < sizeof($usedIndex); $j++) {
          if ($randNum == $usedIndex[$j]) $conflict = true;
        }
        if (!$conflict) break;
      }
      $usedIndex[sizeof($usedIndex)] = $randNum;
      $q[$i] = $r[$randNum];
    }

    $tmp = ''; $th = pack("vvv", sizeof($q), date("Y"), date("md"));
    // 0x6 -> questionCount(0x2), dateY(0x2), dateMD(0x2)
    $current = $base = 0x0 + sizeof($q) * 2 + 6;

    for ($i = 0; $i < sizeof($q); $i++) {
      $qq = explode("`", $q[$i]);
      //if (sizeof($qq) < 12) continue;
      //if ($qq[7] == 0 || sizeof($qq[6]) != 1) continue;
      $subj = $qq[0]; $x = $qq[1]; $a = $qq[2]; $b = $qq[3]; $c = $qq[4]; $d = $qq[5]; $r = $qq[6];
      // $xl= $qq[7] - 0; $al= $qq[8] - 0; $bl= $qq[9] - 0; $cl= $qq[10] - 0; $dl= $qq[11] - 0;
      // DO NOT rely on excel's statistics!
      // Maybe I will change something after change it into csv...
      // So let's calc it out here. NOT strlen!!
      $xl = strlen($x); $al = strlen($a); $bl = strlen($b); $cl = strlen($c); $dl = strlen($d);

      if ($subj == "语文" || $subj == "\xD3\xEF\xCE\xC4") $subj = 1;
      elseif ($subj == "数学" || $subj == "\xCA\xFD\xD1\xA7") $subj = 2;
      elseif ($subj == "英语" || $subj == "\xD3\xA2\xD3\xEF") $subj = 3;
      elseif ($subj == "物理" || $subj == "\xCE\xEF\xC0\xED") $subj = 4;
      elseif ($subj == "化学" || $subj == "\xBB\xAF\xD1\xA7") $subj = 5;
      elseif ($subj == "生物" || $subj == "\xC9\xFA\xCE\xEF") $subj = 6;
      else $subj = 0;

      if ($r == "A" || $r == "a") $r = 0;
      elseif ($r == "B" || $r == "b") $r = 1;
      elseif ($r == "C" || $r == "c") $r = 2;
      elseif ($r == "D" || $r == "d") $r = 3;
      else $r -= 0;
      $tmp.= pack("ccccccc", $subj, $xl, $al, $bl, $cl, $dl, $r);
      $tmp.=$x.$a.$b.$c.$d; // excel's csv is already gb2312
      //$tmp.= mb_convert_encoding($x, 'gbk', 'utf-8');
      //$tmp.= mb_convert_encoding($a, 'gbk', 'utf-8');
      //$tmp.= mb_convert_encoding($b, 'gbk', 'utf-8');
      //$tmp.= mb_convert_encoding($c, 'gbk', 'utf-8');
      //$tmp.= mb_convert_encoding($d, 'gbk', 'utf-8');
      $th .= pack("v", $current);
      $current += $xl + $al + $bl + $cl + $dl + 0x7;
    }
    header('Content-Type: application/octet-stream');
    header('Content-Disposition: attachment; filename="66tb.bin"');
    die($th.$tmp);
  }
  //echo("Err: ".$_FILES["file"]["error"]."<br>");
?>
<form method="post" enctype="multipart/form-data">
  <!--textarea name="str"> </textarea-->
  <label for="file">66tb csv file with splition of `: </label>
  <input type="file" name="file" id="file">
  <input type="hidden" name="flag" value="post">
  <br>
  <input value="ok" type="submit">
</form>
