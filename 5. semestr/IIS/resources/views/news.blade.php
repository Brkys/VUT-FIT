@extends('layouts.app')

@section('title', 'Novinky')

@section('navpanel')
    @parent
@endsection

@section('content')
<div class="container" style="padding-top: 100px; width: 80% !important;"> 
    <input class="form-control" id="myInput" type="text" placeholder="Vyhledat událost.." onkeyup="filter()">
    <table class="table table-hover table-dark text-centered" id="filterTable">
        <thead>
            <tr>
                <th scope="col" style="width: 20%;">Datum</th>
                <th scope="col" style="width: 20%;">Nadpis</th>
                <th scope="col" style="width: 60%;">Oznámení</th>
            </tr>
        </thead>
        <tbody>
          @if(!empty($news))
            @foreach($news as $new)
            <tr>
                <td>{{$new['date']}}</td>
                <td>{{$new['title']}}</td>
                <td>{{$new['content']}}</td>
            </tr>
            @endforeach
          @endif
        </tbody>
    </table>
</div>

<script>
function filter() { 
  var input, filter, table, tr, td, i, txtValue;
  input = document.getElementById("myInput");
  filter = input.value.toUpperCase();
  table = document.getElementById("filterTable");
  tr = table.getElementsByTagName("tr");
  for (i = 0; i < tr.length; i++) {
    td = tr[i].getElementsByTagName("td")[2];
    if (td) {
      txtValue = td.textContent || td.innerText;
      if (txtValue.toUpperCase().indexOf(filter) > -1) {
        tr[i].style.display = "";
      } else {
        tr[i].style.display = "none";
      }
    } 
  }
}
</script>
@endsection