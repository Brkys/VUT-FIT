@extends('layouts.app')

@section('title', 'Volní uživatelé')

@section('navpanel')
    @parent
@endsection

@section('content')
<div class="container" style="padding-top: 100px; width: 80% !important;"> 
    <input class="form-control" id="myInput" type="text" placeholder="Vyhledat podle jména.." onkeyup="filter()">
    <table class="table table-hover table-dark text-centered" id="filterTable">
        <thead>
            <tr>
                <th scope="col" style="width: 40%;">Krycí jméno</th>
                <th scope="col" style="width: 30%;">Civilní jméno</th>
                <th scope="col" style="width: 30%;">Pozvat</th>
            </tr>
        </thead>
        <tbody>
            
           @if(!empty($freeUsers))
               @foreach($freeUsers as $user)
                        <tr>
                             <td>{{$user['name']}}</td>
                             <td>{{$user['fullname']}}</td>
                             @if($user['invited'])
                             <td>Pozvánka odeslána</td>
                             @else
                             <td>
                                <form action="free-users" method="post">
                                    @csrf
                                    <input type="hidden" value="{{$user['id']}}" name="id">
                                    <button type="submit" class="btn btn-primary" name="inv">Pozvat</button>
                                </form>
                             </td>
                             @endif
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
    td = tr[i].getElementsByTagName("td")[0];
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