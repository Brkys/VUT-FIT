@extends('layouts.app')

@section('title', 'Přiřazení kriminální činnosti')

@section('navpanel')
	@parent
@endsection

@section('content')
<div style="padding-top: 100px"></div>
<div class="container" style="; width: 80% !important; background-color: white !important;">
  <form action="assign-criminal" method="post">
    @csrf
  	<h4 style="padding-top: 20px;">Kriminální činnost</h4>
      <select style="width: 100%;" name="action_id">
      @foreach($actions as $action)
  	    <option value="{{$action->ID}}">{{$action->ID}} - {{$action->TypCinnosti}}</option>
      @endforeach
    </select>
  	<div style="padding-top: 20px"></div>
    <input class="form-control" id="myInput" type="text" placeholder="Vyhledat podle jména.." onkeyup="filter()">
    <table class="table table-hover table-dark text-centered" id="filterTable">
        <thead>
            <tr>
                <th scope="col" style="width: 70%;">Jméno</th>
                <th scope="col" style="width: 30%;">Přiřadit</th>
            </tr>
        </thead>
        <tbody>
          @foreach($users as $user)
            @if($user->permission < 4)
              <tr>
                  <td>{{$user->full_name}}</td>
                  <td>
                    <input type="hidden" value="{{$user->id}}" name="user_id">
                    <button type="submit" class="btn btn-primary" name="assignCriminal">Přiřadit</button>
                  </td>
              </tr>
            @endif
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