@extends('layouts.app')

@section('title', 'Aliance')

@section('navpanel')
	@parent
@endsection

@section('content')
<div style="padding-top: 100px"></div>
<div class="container" style="; width: 80% !important; background-color: white !important;">
  <form action="aliance-request" method="post">
    @csrf
  	<h4 style="padding-top: 20px;">Aliance</h4>
  	<div style="padding-top: 20px"></div>
    <input class="form-control" id="myInput" type="text" placeholder="Vyhledat podle familie.." onkeyup="filter()">
    <table class="table table-hover table-dark text-centered" id="filterTable">
        <thead>
            <tr>
                <th scope="col" style="width: 70%;">Familie</th>
                <th scope="col" style="width: 30%;"></th>
            </tr>
        </thead>
        <tbody>
        	@foreach($familie as $familia)
              <tr>
                  <td>{{ $familia->JmenoFamilie }}</td>
                  <td>
                    <input type="hidden" value="{{ $familia->ID_Familie }}" name="familia_id">
                    <button type="submit" class="btn btn-primary" name="aliance-request">Nabídnout alianci</button>
                  </td>
              </tr>
            @endforeach
        </tbody>
    </table>
  </form> 
  <div style="padding-top: 20px"></div>
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