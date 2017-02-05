<?php

  if (@$_POST['str']) {
    $p = $_POST['str'];
    $q = explode("\r\n", $p);
    $tmp = ''; $th = pack("v", sizeof($q));
    $current = $base = 0x0 + sizeof($q) * 2 + 2;
    for ($i = 0; $i < sizeof($q); $i++) {
      $qq = explode("`", $q[$i]);
      $id = $qq[0]; $x = $qq[1]; $a = $qq[2]; $b = $qq[3]; $c = $qq[4]; $d = $qq[5]; $r = $qq[6] - 0;
      $xl= $qq[7] - 0; $al= $qq[8] - 0; $bl= $qq[9] - 0; $cl= $qq[10] - 0; $dl= $qq[11] - 0;
      $tmp.= pack("cccccc", $xl, $al, $bl, $cl, $dl, $r);
      $tmp.= mb_convert_encoding($x, 'gbk', 'utf-8');
      $tmp.= mb_convert_encoding($a, 'gbk', 'utf-8');
      $tmp.= mb_convert_encoding($b, 'gbk', 'utf-8');
      $tmp.= mb_convert_encoding($c, 'gbk', 'utf-8');
      $tmp.= mb_convert_encoding($d, 'gbk', 'utf-8');
      $th .= pack("v", $current);
      $current += $xl + $al + $bl + $cl + $dl + 0x6;
    }

    die($th.$tmp);
  }

?>
<form method="post">
  <textarea name="str"> </textarea>
  <input value="ok" type="submit">
</form>
