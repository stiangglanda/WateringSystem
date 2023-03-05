<?php
$hostName = "localhost";
$username = "root";
$password = "";
$databaseName = "wateringsystemdb";
$conn = new PDO("mysql:host=$hostName;dbname=$databaseName", $username, $password);
$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

try {

$query = "select MD_id, data, created from MoistureData";
$prepared = $conn->prepare($query);
$prepared->execute();
$result = $prepared -> fetchAll(PDO::FETCH_ASSOC);
 ?>
 <table border="1" cellpadding="10" cellspacing="0">
 <?php
 $sn=1;
 foreach($result as $data) {
   
   ?>
    <tr>
   <td><?php echo $data['MD_id']; ?> </td>
   <td><?php echo $data['data']; ?> </td>
   <td><?php echo $data['created']; ?> </td>
    </tr>
    <?php
  }
  ?>
</table>
  <?php
} catch(PDOException $e) {
  echo "Error: " . $e->getMessage();
}
?>